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

void saveData()
{
  EEPROM.put(ADRDIST, dist);
  EEPROM.put(ADRWHUSED, Wh_used);
  EEPROM.put(ADRVMAX, vmax);
  EEPROM.put(ADRMVTIME, moving_time);
  EEPROM.put(ADRTRIP, trip);
  EEPROM.put(ADRWHUSEDKM, Wh_used_trp);
  EEPROM.put(ADRPMAX, powermax);
  EEPROM.put(ADRIMAX, imax);
  EEPROM.put(ADRMAHUSED, mah_used);
  EEPROM.put(ADRWHUSEDKMDTG, Wh_used_dtg);
  EEPROM.put(ADRTRIPDTG, trip_dtg);
  EEPROM.put(ADRTOTMAHUSED, total_ah_used);

  if (getCurrent() < 1.5)
    EEPROM.put(ADRLASTVOLTAGE, getBatRawVoltage());
#ifdef MTG
  EEPROM.put(ADRWHUSEDMTG, Wh_used_mtg);
  EEPROM.put(ADRMVTIMEMTG, moving_time_mtg);
  EEPROM.put(ADRTOTMH, total_mh);
#endif

  // ESP32: Zapisujemy wszystko do Flasha jedną komendą na koniec!
  EEPROM.commit(); 
}

#ifdef INITIAL
void initialConfigSave()
{
  EEPROM.put(ADRFIRSTTIME, FIRST_TIME_CODE);

  // used in savaData(); and loadData();
  EEPROM.put(ADRDIST, 0.0f);
  EEPROM.put(ADRWHUSED, 0.0f);
  EEPROM.put(ADRVMAX, 0.0f);
  EEPROM.put(ADRMVTIME, 0.0f);
  EEPROM.put(ADRTRIP, 0.0f);
  EEPROM.put(ADRWHUSEDKM, 0.0f);
  EEPROM.put(ADRPMAX, 0.0f);
  EEPROM.put(ADRIMAX, 0.0f);
  EEPROM.put(ADRMAHUSED, 0.0f);
  EEPROM.put(ADRWHUSEDKMDTG, 0.0f);
  EEPROM.put(ADRTRIPDTG, 0.0f);
  EEPROM.put(ADRTOTMAHUSED, 0.0f);
  EEPROM.put(ADRLASTVOLTAGE, 95.0f);
  EEPROM.put(ADRWHUSEDMTG, 0.0f);
  EEPROM.put(ADRMVTIMEMTG, 0.0f);
  EEPROM.put(ADRTOTMH, 0.0f);

  EEPROM.put(ADR0CURRENT, 0.0f);
  EEPROM.put(ADR_WATCHDOGRESET, 0);

#ifdef INIT_3000W

  EEPROM.put(ADR_ASSISTMODE, 3);
  EEPROM.put(ADR_BATCAP_AH, 192);
  EEPROM.put(ADR_BATCAP_WH, 1114);
  EEPROM.put(ADR_LVC, 390);
  EEPROM.put(ADR_FULL_BATT_V, 540);
  EEPROM.put(ADR_MVPERA, 10);
  EEPROM.put(ADR_CURDIR, 1);
  EEPROM.put(ADR_VOL_DIV, 33058);
  EEPROM.put(ADR_CUR_SENSOR_OK, 0);
  EEPROM.put(ADR_CUR_PROT, 2);

  EEPROM.put(ADR_TOT_MIN, 85);
  EEPROM.put(ADR_TOT_MAX, 350);
  EEPROM.put(ADR_TIN_MIN, 90);
  EEPROM.put(ADR_TIN_MAX, 360);
  EEPROM.put(ADR_THR_RESET, 1);
  EEPROM.put(ADR_THR_SAFE_VOLTAGE, 370);

  EEPROM.put(ADR_KPHMPH, 0);
  EEPROM.put(ADR_PERIMETER, 2163);
  EEPROM.put(ADR_MOT_MAG, 46);
  EEPROM.put(ADR_GEAR_RATIO, 10);

  EEPROM.put(ADR_BT_BUTTONS, 0);
  EEPROM.put(ADR_EBRAKEHILO, 0);

  EEPROM.put(ADR_TEMPCTEMPF, 0);
  EEPROM.put(ADR_TEMPTYPE1, 3);
  EEPROM.put(ADR_TEMPTYPE2, 1);
  EEPROM.put(ADR_OVHT1, 140);
  EEPROM.put(ADR_OVHT2, 60);

  EEPROM.put(ADR_POWERKP, 150);
  EEPROM.put(ADR_POWERKI, 80);
  EEPROM.put(ADR_POWERKD, 50);
  EEPROM.put(ADR_P_LOW, 0);
  EEPROM.put(ADR_I_LOW, 0);
  EEPROM.put(ADR_D_LOW, 0);
  EEPROM.put(ADR_LOW_THRESHOLD, 0);

  EEPROM.put(ADR_SPEEDFACTORMIN, 1);

  EEPROM.put(ADR_PIDPWMMAX, 200);

  EEPROM.put(ADR_SPEEDFACTOR_RAMP_UP, 40);
  EEPROM.put(ADR_CRUISE_CONTROL_POWER_MIN, 240);
  EEPROM.put(ADR_CRUISE_CONTROL_POWER_MAX, 1300);
  EEPROM.put(ADR_CRUISE_CONTROL_POWER_RAMP_UP, 300);
  EEPROM.put(ADR_CRUISE_CONTROL_MAX_SPEED, 40);

  EEPROM.put(ADR_AUTOLEGAL, 1);
  EEPROM.put(ADR_LEGALLIMIT_ON_OFF, 1);
  EEPROM.put(ADR_LEGALLIMIT_SPEED, 25);
  EEPROM.put(ADR_LEGALLIMIT_POWER, 250);
  EEPROM.put(ADR_PASMAGNETS, 12);
  EEPROM.put(ADR_PWR_LIM_PAS_1, 100);
  EEPROM.put(ADR_PWR_LIM_PAS_2, 180);
  EEPROM.put(ADR_PWR_LIM_PAS_3, 250);
  EEPROM.put(ADR_PWR_LIM_PAS_4, 350);
  EEPROM.put(ADR_PWR_LIM_PAS_5, 600);
  EEPROM.put(ADR_SPD_LIM_PAS_1, 20);
  EEPROM.put(ADR_SPD_LIM_PAS_2, 25);
  EEPROM.put(ADR_SPD_LIM_PAS_3, 25);
  EEPROM.put(ADR_SPD_LIM_PAS_4, 30);
  EEPROM.put(ADR_SPD_LIM_PAS_5, 38);
  EEPROM.put(ADR_MIN_SPD_PAS_1, 0);
  EEPROM.put(ADR_MIN_SPD_PAS_2, 0);
  EEPROM.put(ADR_MIN_SPD_PAS_3, 0);
  EEPROM.put(ADR_MIN_SPD_PAS_4, 0);
  EEPROM.put(ADR_MIN_SPD_PAS_5, 0);
  EEPROM.put(ADR_CAD_MIN_PAS_1, 0);
  EEPROM.put(ADR_CAD_MIN_PAS_2, 0);
  EEPROM.put(ADR_CAD_MIN_PAS_3, 0);
  EEPROM.put(ADR_CAD_MIN_PAS_4, 0);
  EEPROM.put(ADR_CAD_MIN_PAS_5, 0);
  EEPROM.put(ADR_CAD_MAX_PAS_1, 10);
  EEPROM.put(ADR_CAD_MAX_PAS_2, 10);
  EEPROM.put(ADR_CAD_MAX_PAS_3, 10);
  EEPROM.put(ADR_CAD_MAX_PAS_4, 10);
  EEPROM.put(ADR_CAD_MAX_PAS_5, 10);
  EEPROM.put(ADR_RAMP_UP_PAS_1, 300);
  EEPROM.put(ADR_RAMP_UP_PAS_2, 300);
  EEPROM.put(ADR_RAMP_UP_PAS_3, 300);
  EEPROM.put(ADR_RAMP_UP_PAS_4, 400);
  EEPROM.put(ADR_RAMP_UP_PAS_5, 500);

  EEPROM.put(ADR_BOOST_POWER_PAS_1, 500);
  EEPROM.put(ADR_BOOST_POWER_PAS_2, 500);
  EEPROM.put(ADR_BOOST_POWER_PAS_3, 750);
  EEPROM.put(ADR_BOOST_POWER_PAS_4, 800);
  EEPROM.put(ADR_BOOST_POWER_PAS_5, 1000);
  EEPROM.put(ADR_BOOST_TIME_PAS_1, 3500);
  EEPROM.put(ADR_BOOST_TIME_PAS_2, 3500);
  EEPROM.put(ADR_BOOST_TIME_PAS_3, 3500);
  EEPROM.put(ADR_BOOST_TIME_PAS_4, 3500);
  EEPROM.put(ADR_BOOST_TIME_PAS_5, 3500);
  EEPROM.put(ADR_BOOST_SPEED_PAS_1, 10);
  EEPROM.put(ADR_BOOST_SPEED_PAS_2, 10);
  EEPROM.put(ADR_BOOST_SPEED_PAS_3, 10);
  EEPROM.put
