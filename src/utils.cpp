#include "utils.h"
#include "global_vars.h"

// ------------------------ Global variables ------------------------
byte buttonsPressed;     // Current button state (bitmask)
boolean firstDigit;      // Toggle for 7-segment display
byte lockTime = 0;       // Counter to auto-return to HOME

// ------------------------ Button handling ------------------------
void readBtns() {
  buttonsPressed = 0b000;

  if (digitalRead(LOCK_BTN) == LOW) {
    delay(DEBOUNCE);
    bitSet(buttonsPressed, 2); // LOCK button pressed
    return;
  }

  if (digitalRead(DOWN_BTN) == LOW) {
    delay(DEBOUNCE);
    bitSet(buttonsPressed, 1); // DOWN button pressed
    return;
  }

  if (digitalRead(UP_BTN) == LOW) {
    delay(DEBOUNCE);
    bitSet(buttonsPressed, 0); // UP button pressed
    return;
  }
}

// Check if a specific button is pressed
boolean isPressed(uint8_t btn) {
  if (btn == LOCK_BTN) return bitRead(buttonsPressed, 2);
  if (btn == DOWN_BTN) return bitRead(buttonsPressed, 1);
  if (btn == UP_BTN) return bitRead(buttonsPressed, 0);
  return false;
}

// ------------------------ Time update ------------------------
void updateTime() {
  // Store previous time/temperatureNow values
  prevSecond = secondNow;
  prevHour = hourNow;
  prevMinute = minuteNow;
  prevDay = dayNow;
  prevMonth = monthNow;
  prevYear = yearNow;
  prevTemperature = temperatureNow;
  prevDayOfWeek = dayOfWeekNow;

  // Update current time/temperatureNow from RTC
  secondNow = myRTC.getSecond();
  hourNow = myRTC.getHour(use12hFormat, pmFlag);
  minuteNow = myRTC.getMinute();
  dayNow = myRTC.getDate();
  monthNow = myRTC.getMonth(centuryFlag);
  yearNow = myRTC.getYear() + 1970;
  temperatureNow = (byte)roundf(myRTC.getTemperature());
  dayOfWeekNow = myRTC.getDoW();
}

// ------------------------ Alarm checking ------------------------
void checkAndTriggerAlarm() {
    for (byte i = 0; i < eeprom.alarmCount; i++) {
        if (bitRead(eeprom.alarms[i].days, 0) &&       // Alarm active
            eeprom.alarms[i].hour == hourNow &&            // Hour matches
            eeprom.alarms[i].minute == minuteNow &&          // Minute matches
            bitRead(eeprom.alarms[i].days, 8 - dayOfWeekNow)) // Day matches
        {
            if (eeprom.programType == 0) {
                duration = eeprom.alarms[i].duration; // Retrieve duration
            } else {
                digitalWrite(RELAY, eeprom.alarms[i].duration == 0 ? HIGH : LOW);
                digitalWrite(LED, eeprom.alarms[i].duration == 0 ? LOW : HIGH);
            }
            break;
        }
    }
}

// ------------------------ 7-segment display ------------------------
void onSevenSegmentDisplayToggle() {
  firstDigit = !firstDigit;
  PORTD |= (1 << DIGIT1 | 1 << DIGIT2); // Enable both digits
  PORTD &= ~ABCD;                       // Clear BCD lines

  if (firstDigit) {
    PORTD |= bcd[duration / 10];           // Tens digit
    PORTD &= ~(1 << DIGIT1);
  } else {
    PORTD |= bcd[duration % 10];           // Units digit
    PORTD &= ~(1 << DIGIT2);
  }
}

// ------------------------ Relay initialization ------------------------
void initRelay() {
  if (eeprom.alarmCount == 0) return;

  int minutes = hourNow * 60 + minuteNow;
  int idx = 0;

  while (minutes < eeprom.alarms[idx].hour * 60 + eeprom.alarms[idx].minute &&
         idx < eeprom.alarmCount) {
    idx++;
  }

  idx--;
  if (idx < 0) idx = eeprom.alarmCount - 1;

  if (bitRead(eeprom.alarms[idx].days, 0)) {
    digitalWrite(RELAY, eeprom.alarms[idx].duration == 0 ? HIGH : LOW);
    digitalWrite(LED, eeprom.alarms[idx].duration == 0 ? LOW : HIGH);
  }
}

// ------------------------ Code executed every second ------------------------
void everySecond() {
  updateTime();

  // Check alarms only when minute changes and state != 0
  if (minuteNow != prevMinute && eeprom.state != 0) {
    checkAndTriggerAlarm();
  }

  // Program type 0: countdown mode
  if (eeprom.programType == 0) {
    if (duration == 0) {
      digitalWrite(RELAY, HIGH);
    } else {
      digitalWrite(RELAY, LOW);
      duration--;
      tone(BUZZER, BUZZER_FREQ, 300);
    }
  }

  // Update LCD based on current menu
  if (currentMenu == HOME) refreshHome();

  if (currentMenu == ALARMS) {
    lockTime++;
    if (lockTime > LOCK_TIME) {
      currentMenu = HOME;
      lockTime = 0;
      initHome();
    }
  }
}

// ------------------------ Menu handling ------------------------
void handleMenu() {
  switch (currentMenu) {
    case HOME:
      if (isPressed(LOCK_BTN)) {
        tone(BUZZER, 1500, DEBOUNCE);
        if (eeprom.alarmCount == 0) {
          lcd.clear();
          lcd.home();
          lcd.print("    No alarm    ");
          lcd.setCursor(0, 1);
          lcd.print("   configured   ");
          delay(MSG_DELAY);
          initHome();
        } else {
          currentMenu = ALARMS;
          alarmIndex = 0;
          displayAlarm();
        }
      }
      break;

    case ALARMS:
      if (isPressed(LOCK_BTN)) {
        tone(BUZZER, 1500, DEBOUNCE);
        lockTime = 0;
        currentMenu = HOME;
        initHome();
        delay(DEBOUNCE);
      }
      if (isPressed(UP_BTN)) {
        tone(BUZZER, 1500, DEBOUNCE);
        lockTime = 0;
        alarmIndex = (alarmIndex + 1) % eeprom.alarmCount;
        displayAlarm();
        delay(DEBOUNCE);
      }
      if (isPressed(DOWN_BTN)) {
        tone(BUZZER, 1500, DEBOUNCE);
        lockTime = 0;
        alarmIndex--;
        if (alarmIndex < 0) alarmIndex = eeprom.alarmCount - 1;
        displayAlarm();
        delay(DEBOUNCE);
      }
      break;
  }
}
