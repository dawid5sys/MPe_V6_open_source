/*
 * This file is part of MPe-firmware.
 *
 * MPe-firmware is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MPe-firmware is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MPe-firmware.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2022 Marek Przybylak
 */

#define FIRMWARE_VERSION 6010
#define FIRST_TIME_CODE 12346

// #define INITIAL // Required for new hardware - stores eeprom values and first time CODE)

#ifdef INITIAL
// #define INIT_3000W
// #define INIT_6000W
// #define INIT_10000W
#endif

// CONFIG
#define OLED1 // Enable Oled Display for SSD1306 128x32 0,91"

#ifdef OLED1
#define CONFIG_MENU // Enable built-in Configuration screen/menu (using ~8% of memory !!!)
#endif

#define PAS // Enable PAS support // using ~12% of memory
// #define MTG // Adds Minutes To Go and moto-hours screen //using ~4% of memory
// #define LIGHT_OPERATION // Experimental lights turn on/off functionality. Requires external hardware.

// #define SERIALPLOT // Enable debug printing for plotting
// #define TESTING // OVERRIDES VALUES current,voltage speed etc... for something to display without actually measure anything.
// #define TEST_LOOP_SPEED // Enable debug prints with time needed to iterate over main loop
#define DEBUG // Enables debug messages //using ~1% of memory

// TIMERS
#define T100MS_LOOP 100
#define T30MS_LOOP 30
#define SPEED_COUNT_TIME 250
#define CADENCE_COUNT_TIME 500
#define SCREENREFRESH 602
#define TIME_SEND_UART 497

// REST
#ifdef MTG
#define NUM_SCREEN_POSITIONS 6
#endif

#ifndef MTG
#define NUM_SCREEN_POSITIONS 5
#endif

// --- PINS DIGITAL ESP32 ---
#define PIN_PAS 18             
#define PIN_DSPEED 19          
#define PIN_THROTTLE_OUT 25    // DAC (Prawdziwy analog 0-3.3V)
#define PIN_BUTTON_DOWN 27     
#define PIN_BUTTON_UP 14       
#define PIN_BRAKE 12           

// --- PINS ANALOG ESP32 ---
#define PIN_LM35_TEMP_T1 32    
#define PIN_LM35_TEMP_T2 33    

// --- PINS I2C ESP32 ---
#define PIN_SDA 21             
#define PIN_SCL 22             

// PINS ANALOG ADS1115 (Bez zmian)
#define PIN_V_REFERENCE 0
#define PIN_CURRENT 1
#define PIN_VOLTAGE 2
#define PIN_THROTTLE_IN 3

#define TEMPRESVAL 2200.0
#define TEMPRESVAL2 1000.0

// Steinhart-Hart coefficients
#define A_THERM 0.001129148
#define B_THERM 0.000234125
#define C_THERM 0.0000000876741

#define A_10K 0.001129148
#define B_10K 0.000234125
#define C_10K 0.0000000876741

#define A_KTY83 0.02262021256593017
#define B_KTY83 -0.003444732389272895
#define C_KTY83 0.00001374931587826135

#ifdef DEBUG
#define DEBUG_PR(x) Serial.print(x)
#define DEBUG_PRSC(x) Serial.printsc(x)
#define DEBUG_PRLN(x) Serial.println(x)
#else
#define DEBUG_PR(x)
#define DEBUG_PRLN(x)
#endif

// Configuration
#include "MPe-configuration.h"

// Custom serial print functions
#include "MPe-PrintWithSC.h"
PrintWithSC mySerial(Serial);

// --- EEPROM Zastępstwo dla ESP32 ---
#include <EEPROM.h>
#define EEPROM_SIZE 1024

class ProxyEEPROM {
  public:
    int readInt(int address) {
      int val;
      EEPROM.get(address, val);
      return val;
    }
    void updateInt(int address, int val) {
      int current;
      EEPROM.get(address, current);
      if (current != val) {
        EEPROM.put(address, val);
        EEPROM.commit();
      }
    }
};
ProxyEEPROM mpeEEPROM;

#ifdef PAS
// POWER PID
#include "PID_v1.h"
float pPID_Set, pPID_In, pPID_Out;
PID powerPID(&pPID_In, &pPID_Out, &pPID_Set, 0.1, 0.1, 0.1, 1, 255); // Tymczasowo 255, ustawiane w setup()
#endif

#ifdef OLED1
// DISPLAY
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "DejaVu_LGC_Sans_Mono_Bold_20.h"
Adafruit_SSD1306 display(128, 32, &Wire, -1);
#endif

// ADS1115 ADC 15bit
#include "Adafruit_ADS1015.h"
Adafruit_ADS1115 ads;

// --- WATCHDOG DLA ESP32 ---
#include <esp_task_wdt.h>
#define WDT_TIMEOUT 3

// Expotential Filter
#include "Filter.h"
ExponentialFilter<float> pin_current_mV_filtered(10.0, 2480.0);
ExponentialFilter<float> vRef_filtered(10.0, 5.0);
ExponentialFilter<float> pin_voltage_mV_filtered(20.0, 2800.0);
ExponentialFilter<float> pin_temp1_V_filtered(3.0, 0.3);
ExponentialFilter<float> pin_temp2_V_filtered(3.0, 0.3);
ExponentialFilter<float> pas_rpm_ex(20.0, 0.0);
ExponentialFilter<float> speed_filtered(40.0, 0.0);
ExponentialFilter<float> weightOnPedal_adc_filtered(50.0, 0);
ExponentialFilter<float> humanPower_adc_filtered(15.0, 0);

// --- Deklaracja przerwań w szybkiej pamięci IRAM ---
void IRAM_ATTR mot_rot();
void IRAM_ATTR speed_one();

bool b_up_p = false;
bool b_up_lp = false;
bool b_dwn_p = false;
bool b_dwn_lp = false;
bool b_updwn_p = false;
bool stopped = true;
bool cruisecontrol = false;
bool screenconfig = false;
bool screenconfig_just_entered = false;
bool temperatureOK = true;
bool brake = true;
bool throttle_relased_reset = true;
bool last_brake = true;
#ifdef CONFIG_MENU
bool configchanged = false;
#endif
bool statscreen = false;
bool rideOK = false;
bool pas_after_power_throttle = false;

volatile int rotation_impulses = 0;
#ifdef PAS
volatile int pas_rotation_impulses = 0;
#endif
volatile int speedOneimpulseTime = 0;
volatile bool speed_impulse = false;
volatile unsigned long timer_speed_one = 0;

int screen = 1;
int cruisecontrol_pwm = 0;
#ifdef CONFIG_MENU
int configCursor = 1;
#endif
int last_dtg = 0;
int last_assistmode = 0;
int address = 0;
int pwmOut = 0;
int voltageOut = 80;
int speedFactor = 100;

float powermax = 0.0;
float imax = 0.0;
float Wh_used = 0.0;
float Wh_used_trp = 0.0;
float Wh_used_dtg = 0.0;
float vRef = 5.0;
#ifdef MTG
float Wh_used_mtg = 0.0;
float moving_time_mtg = 0;
float last_mtg = 0.0;
float total_mh = 0.0;
#endif
float mah_used = 0.0;
float total_ah_used = 0.0;
float speed = 0.0;
float vmax = 0.0;
float dist = 0.0;
float trip = 0.0;
float trip_dtg = 0.0;
float moving_time = 0.0;
float cruisecontrol_speed = 0.0;

#ifdef PAS
unsigned long timer_pas_rpm = 0;
#endif
unsigned long timer_100ms_loop = 0;
unsigned long timer_30ms_loop = 0;
unsigned long timer_speed = 0;
unsigned long timer_button = 0;
unsigned long timer_refresh = 0;
unsigned long timer_send_uart = 0;
unsigned long timer_stopped = 0;
unsigned long timer_no_pedalling = 0;
unsigned long timer_adc_offset = 0;
unsigned long timer_value_delay_1 = 0;
unsigned long timer_value_delay_2 = 0;

float adc_offset = 0.0;
bool adc_will_save = 0;
bool adc_saved = 1;

void setup()
{
  Serial.begin(115200); // ESP32 lubi szybszą komunikację
  Serial.setTimeout(50); 

  EEPROM.begin(EEPROM_SIZE); // Inicjalizacja flash dla ESP32

#ifdef PAS
  powerPID.SetOutputLimits(0, mpeEEPROM.readInt(ADR_PIDPWMMAX));
#endif

  // PINMODE
  dacWrite(PIN_THROTTLE_OUT, 0); // Użycie DAC zamiast analogWrite (PWM)

  pinMode(PIN_BUTTON_UP, INPUT_PULLUP);
  pinMode(PIN_BUTTON_DOWN, INPUT_PULLUP);
  pinMode(PIN_BRAKE, INPUT_PULLUP);

  if ((mpeEEPROM.readInt(ADR_MOT_MAG) > 1))
    attachInterrupt(digitalPinToInterrupt(PIN_DSPEED), mot_rot, FALLING);
  else
    attachInterrupt(digitalPinToInterrupt(PIN_DSPEED), speed_one, FALLING);

  pinMode(PIN_LM35_TEMP_T1, INPUT);
  pinMode(PIN_LM35_TEMP_T2, INPUT);

#ifdef OLED1
  // LCD Init
  Wire.begin(PIN_SDA, PIN_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
#endif

  if (mpeEEPROM.readInt(ADRFIRSTTIME) == FIRST_TIME_CODE)
  {
    loadData();
#ifdef INITIAL
    display.clearDisplay();
    display.setFont();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(11, 0);
    display.println("LOAD OK");
    display.display();
    while (true) ;
#endif
  }
#ifndef INITIAL
  else
  {
    while (true) ;
  }
#endif
#ifdef INITIAL
  else
  {
    for (int i = 2; i < 512; i += 2)
    {
      mpeEEPROM.updateInt(i, 55555);
    }
    initialConfigSave();

    display.clearDisplay();
    display.setFont();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(11, 0);
    display.println("INIT OK");
    display.display();
    while (true) ;
  }
#endif

  // ADS INIT
  ads.begin();

  // Acu Full Charge Check & reset
  float vol = 0.0;
  for (int i = 0; i < 3; i++)
    vol = getBatRawVoltage();

  if (vol > (eepromLoadFloat(ADRLASTVOLTAGE) + 2.0))
  {
    resetBattery();
  }

  // TEMP INIT
  pin_temp1_V_filtered.SetCurrent(0.005 * analogRead(PIN_LM35_TEMP_T1));
  pin_temp2_V_filtered.SetCurrent(0.005 * analogRead(PIN_LM35_TEMP_T2));

  mpeEEPROM.updateInt(ADR_PRINTINFO, 1);

#ifdef SERIALPLOT
  mpeEEPROM.updateInt(ADR_SERIAL_PLOT, 0);
#endif

  if (mpeEEPROM.readInt(ADR_AUTOLEGAL) && !mpeEEPROM.readInt(ADR_WATCHDOGRESET))
  {
    mpeEEPROM.updateInt(ADR_LEGALLIMIT_ON_OFF, 1);
  }

  // Watchdog INIT dla ESP32
  esp_task_wdt_init(WDT_TIMEOUT, true); 
  esp_task_wdt_add(NULL);
  mpeEEPROM.updateInt(ADR_WATCHDOGRESET, 0);
}

void loop()
{
  // Reset Watchdoga w ESP32
  esp_task_wdt_reset();

  serialConfig();
  setThrottle();
  checkButtons();

  if ((millis() - timer_100ms_loop) > T100MS_LOOP)
  {
    checkVRef();
    set_adc_offset();
    checkEbrake();
    checkTemp();
    checkIfrideOK();
#ifdef PAS
    checkTorque();
#endif
    timer_100ms_loop = millis();
  }

  speed_one_check();
  checkCurrentAndVoltage();
  countSpeed();
  checkIfStopped();

#ifdef PAS
  checkCadence();
#endif

  screenRefresh();
  serialPrintInfo();
}
