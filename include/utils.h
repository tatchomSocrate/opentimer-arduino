#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

#define LOCK_BTN A0
#define DOWN_BTN A1
#define UP_BTN A2

#define MSG_DELAY 2000 // Message display duration
#define DEBOUNCE 300   // 300 ms delay to eliminate button bounce effects

#define BUZZER 8
#define LED 13
#define BUZZER_FREQ 2000
#define RELAY 9

#define DIGIT1 3
#define DIGIT2 2

// 7-segment binary inputs
#define A PD7
#define B PD5
#define C PD4
#define D PD6

#define LOCK_TIME 20 // Display unlock duration

void readBtns();
boolean isPressed(uint8_t btn);
void updateTime();
void checkAndTriggerAlarm();
void onSevenSegmentDisplayToggle();
void everySecond();
void initRelay();
void handleMenu();

#endif
