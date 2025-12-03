#ifndef VARS_H
#define VARS_H

#include "database.h"
#include "datatypes.h"
#include "display.h"
#include "utils.h"

#include <DS3231.h>
#include <LiquidCrystal_I2C.h>

// Global EEPROM memory image
extern EEPROMData eeprom;

// Temporary byte buffer
extern byte tempByte;

// LCD instance
extern LiquidCrystal_I2C lcd;

// Currently selected alarm index
extern int alarmIndex;

// Current time/date values
extern byte hourNow, minuteNow, secondNow;
extern byte dayNow, monthNow, dayOfWeekNow;
extern unsigned int yearNow;

// Previous time/date snapshot (for UI refresh)
extern byte prevHour, prevMinute, prevSecond;
extern byte prevDay, prevMonth, prevDayOfWeek;
extern unsigned int prevYear;

// Current temperatureNow and previous temperatureNow
extern byte temperatureNow;
extern byte prevTemperature;

// RTC module instance
extern DS3231 myRTC;

// RTC flags
extern bool centuryFlag;
extern bool use12hFormat; // Whether the clock is in 12-hour format
extern bool pmFlag;       // AM/PM indicator when in 12h mode

// Alarm duration (in minutes)
extern byte duration;

// Current LCD menu page
extern MenuLcd currentMenu;

// Request timeout management
extern int timeoutCounter;
extern boolean timeoutEnabled;

#endif
