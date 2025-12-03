#define RELEASE // Comment to initialize EEPROM memory (default password)

#include "database.h"
#include "display.h"
#include "global_vars.h"
#include "server.h"
#include "utils.h"
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// -----------------------------------------------------------------------------
// Custom LCD characters
// -----------------------------------------------------------------------------
byte thermometerChar[] = {B00100, B01010, B01010, B01010, B01110, B11111, B11111, B01110};
byte nextChar[]       = {B01000, B01100, B01110, B01111, B01110, B01100, B01000, B00000};
byte prevChar[]       = {B00010, B00110, B01110, B11110, B01110, B00110, B00010, B00000};
byte lineChar[]       = {B10000, B10000, B10000, B10000, B10000, B10000, B10000, B10000};
byte onChar[]         = {B00000, B00000, B00000, B00001, B00010, B10100, B01000, B00000};
byte offChar[]        = {B00000, B00000, B10001, B01010, B00100, B01010, B10001, B00000};

// Current menu on LCD
MenuLcd currentMenu = HOME;

// -----------------------------------------------------------------------------
// Global variables
// -----------------------------------------------------------------------------
EEPROMData eeprom;
byte tempByte;
int alarmIndex;
byte hourNow, minuteNow, secondNow, dayNow, monthNow, dayOfWeekNow;
byte prevHour, prevMinute, prevSecond, prevDay, prevMonth, prevDayOfWeek;
unsigned int yearNow, prevYear;
byte temperatureNow, prevTemperature;
DS3231 myRTC;
bool century;
bool use12hFormat; // 12-hour format flag
bool pmFlag;  // PM flag for 12-hour format
byte duration;
int timeoutCounter;
boolean timeoutEnabled;
boolean flag = true;
int count;

// LCD object
LiquidCrystal_I2C lcd(0x27, 16, 2); // 16x2 LCD at address 0x27

// -----------------------------------------------------------------------------
// Setup for EEPROM Initialization (default password / config)
// -----------------------------------------------------------------------------
#ifndef RELEASE
void setup() {
  store(PROGRAM_TYPE_ADR, 0);
  store(NBR_ALARMS_ADR, 0);
  store(DESCRIPTION_LEN_ADR, 0);
  store(AUTHOR_LEN_ADR, 0);
  store(STATE_ADR, 0);
  byte defaultPassword[32] = { // sha256 of "0000"
      0x9a,0xf1,0x5b,0x33,0x6e,0x6a,0x96,0x19,
      0x92,0x85,0x37,0xdf,0x30,0xb2,0xe6,0xa2,
      0x37,0x65,0x69,0xfc,0xf9,0xd7,0xe7,0x73,
      0xec,0xce,0xde,0x65,0x60,0x65,0x29,0xa0};
  for (byte i = 0; i < 32; i++) {
    store(PASSWORD_ADR + i, defaultPassword[i]);
  }
}
void loop() {}
#else

// -----------------------------------------------------------------------------
// Setup function
// -----------------------------------------------------------------------------
void setup() {
  cli(); // Disable global interrupts

  // Configure Timer1 in CTC (Clear Timer on Compare Match) mode
  TCCR1A = 0;
  TCCR1B = 0;
  bitSet(TCCR1B, WGM12);               // CTC mode
  TCCR1B |= (1 << CS11) | (1 << CS10); // Prescaler = 64
  OCR1A = DISPLAY_COUNT;                // 249 for 1ms interrupt
  bitSet(TIMSK1, OCIE1A);               // Enable Timer1 Compare Match interrupt
  sei();                                // Enable global interrupts

  // Set ABCD pins as outputs
  DDRD |= ABCD;

  // Initialize buttons
  pinMode(LOCK_BTN, INPUT_PULLUP);
  pinMode(DOWN_BTN, INPUT_PULLUP);
  pinMode(UP_BTN, INPUT_PULLUP);

  // Initialize buzzer, LED, relay, 7-segment digits
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(DIGIT1, OUTPUT);
  pinMode(DIGIT2, OUTPUT);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.createChar(THERMOMETER_CHAR, thermometerChar);
  lcd.createChar(NEXT_CHAR, nextChar);
  lcd.createChar(PREV_CHAR, prevChar);
  lcd.createChar(LINE_CHAR, lineChar);
  lcd.createChar(ON_CHAR, onChar);
  lcd.createChar(OFF_CHAR, offChar);

  lcd.home();
  lcd.print("    OpenTimer   ");

  Serial.begin(9600);
  Wire.begin(); // Start I2C

  // Load data from EEPROM
  getProgramType();
  getAlarms();
  getDescription();
  getAuthor();
  getPassword();
  getState();
  serialFlush();
  disableTimeout(); // Disable timeout initially

  delay(MSG_DELAY);
  updateTime();
  initHome();

  if (eeprom.programType == 1) {
    initRelay();
  }
}

// -----------------------------------------------------------------------------
// Timer1 ISR (1ms interval)
// -----------------------------------------------------------------------------
ISR(TIMER1_COMPA_vect) {
  onSevenSegmentDisplayToggle(); // Toggle 7-segment display
  count = (count + 1) % 1000;
  if (count == 0) flag = true;

  // Handle timeout countdown
  if (timeoutEnabled) {
    timeoutCounter--;
    if (timeoutCounter == 0) timeOut();
  }
}

// -----------------------------------------------------------------------------
// Main loop
// -----------------------------------------------------------------------------
void loop() {
  readBtns(); // Read buttons
  if (flag) {
    flag = false;
    everySecond(); // Update every second tasks
  }
  handleMenu();   // Handle LCD menu interactions
  execRequest();  // Handle Bluetooth requests
}

#endif