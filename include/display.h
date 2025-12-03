#ifndef DISPLAY_H
#define DISPLAY_H

#include "utils.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// ============================================================================
//   Days of the week abbreviations
// ============================================================================
const char DaysOfWeek[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// ============================================================================
//   AlarmState strings for display
// ============================================================================
const char AlarmState[][4] = {"OFF", " ON"};

// Custom LCD characters
enum LcdCustomChars {
    NEXT_CHAR,          // Next arrow
    PREV_CHAR,          // Previous arrow
    THERMOMETER_CHAR,   // Thermometer symbol
    LINE_CHAR,          // Vertical line
    ON_CHAR,            // ON symbol
    OFF_CHAR,           // OFF symbol
    DEGRE_CHAR = 223    // Degree symbol (ASCII code 223)
};

// LCD menu pages
enum MenuLcd { HOME, ALARMS };

// BCD representation
const byte bcd[10] = {
    0,
    1 << A,
    1 << B,
    (1 << A) | (1 << B),
    1 << C,
    (1 << A) | (1 << C),
    (1 << B) | (1 << C),
    (1 << A) | (1 << B) | (1 << C),
    1 << D,
    (1 << A) | (1 << D)
};

// Mask for 7-segments binary input (A + B + C + D)
const uint32_t ABCD = (1 << A) | (1 << B) | (1 << C) | (1 << D);

// ============================================================================
//   Function prototypes for LCD display
// ============================================================================
void displayAlarm();   // Display alarm info on the LCD
void refreshHome();    // Refresh the main/home screen
void initHome();       // Initialize home screen layout

#endif
