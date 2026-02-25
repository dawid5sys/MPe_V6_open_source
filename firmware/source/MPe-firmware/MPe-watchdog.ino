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

#include <esp_system.h>

void enable_watchdog()
{
  // W ESP32 Watchdog został już uruchomiony w setup() w głównym pliku MPe-firmware.ino.
  // Tutaj weryfikujemy, co było powodem ostatniego resetu procesora.

  esp_reset_reason_t reset_reason = esp_reset_reason();

  // Sprawdzamy, czy reset nastąpił z powodu Watchdoga
  if (reset_reason == ESP_RST_WDT || reset_reason == ESP_RST_TASK_WDT || reset_reason == ESP_RST_INT_WDT)
  {
    // Zapisujemy flagę błędu do emulowanego EEPROM dokładnie tak, jak robiło to ISR na Atmędze
    mpeEEPROM.updateInt(ADR_WATCHDOGRESET, 1);
    saveData();
  }
}
