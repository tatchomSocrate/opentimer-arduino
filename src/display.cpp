#include "display.h"
#include "global_vars.h"

// -----------------------------------------------------------------------------
// Initialize the home screen with current date, time, state, and temperatureNow
// -----------------------------------------------------------------------------
void initHome() {
  lcd.clear();
  lcd.home();

  // Display day of week and date
  lcd.setCursor(1, 0);
  lcd.print(DaysOfWeek[dayOfWeekNow - 1]);
  lcd.print(',');
  lcd.print(dayNow / 10);
  lcd.print(dayNow % 10);
  lcd.print('/');
  lcd.print(monthNow / 10);
  lcd.print(monthNow % 10);
  lcd.print('/');
  lcd.print((yearNow / 1000) % 10);
  lcd.print((yearNow / 100) % 10);
  lcd.print((yearNow / 10) % 10);
  lcd.print(yearNow % 10);

  // Display current time
  lcd.setCursor(0, 1);
  lcd.print(hourNow / 10);
  lcd.print(hourNow % 10);
  lcd.print(':');
  lcd.print(minuteNow / 10);
  lcd.print(minuteNow % 10);
  lcd.print(':');
  lcd.print(secondNow / 10);
  lcd.print(secondNow % 10);

  // Display system state (ON/OFF) and temperatureNow
  lcd.print(' ');
  lcd.write(eeprom.state ? ON_CHAR : OFF_CHAR);
  lcd.print(' ');
  lcd.write(THERMOMETER_CHAR);
  lcd.print((byte)temperatureNow / 10);
  lcd.print((byte)temperatureNow % 10);
  lcd.write(DEGRE_CHAR);
  lcd.print('C');
}

// -----------------------------------------------------------------------------
// Refresh only changed elements on the home screen to optimize LCD updates
// -----------------------------------------------------------------------------
void refreshHome() {
  if (dayOfWeekNow != prevDayOfWeek) {
    lcd.setCursor(1, 0);
    lcd.print(DaysOfWeek[dayOfWeekNow - 1]);
  }
  if (dayNow != prevDay) {
    lcd.setCursor(5, 0);
    lcd.print(dayNow / 10);
    lcd.print(dayNow % 10);
  }
  if (monthNow != prevMonth) {
    lcd.setCursor(8, 0);
    lcd.print(monthNow / 10);
    lcd.print(monthNow % 10);
  }
  if (yearNow != prevYear) {
    lcd.setCursor(11, 0);
    lcd.print((yearNow / 1000) % 10);
    lcd.print((yearNow / 100) % 10);
    lcd.print((yearNow / 10) % 10);
    lcd.print(yearNow % 10);
  }
  if (hourNow != prevHour) {
    lcd.setCursor(0, 1);
    lcd.print(hourNow / 10);
    lcd.print(hourNow % 10);
  }
  if (minuteNow != prevMinute) {
    lcd.setCursor(3, 1);
    lcd.print(minuteNow / 10);
    lcd.print(minuteNow % 10);
  }
  if (secondNow != prevSecond) {
    lcd.setCursor(6, 1);
    lcd.print(secondNow / 10);
    lcd.print(secondNow % 10);
  }
  if (temperatureNow != prevTemperature) {
    lcd.setCursor(12, 1);
    lcd.print(temperatureNow / 10);
    lcd.print(temperatureNow % 10);
  }
}

// -----------------------------------------------------------------------------
// Display the currently selected alarm on the LCD
// -----------------------------------------------------------------------------
void displayAlarm() {
  lcd.clear();
  lcd.home();

  // Display alarm index
  lcd.print('n');
  lcd.write(DEGRE_CHAR);
  lcd.print((alarmIndex + 1) / 10);
  lcd.print((alarmIndex + 1) % 10);
  lcd.print("  ");

  // Display alarm time
  lcd.print(eeprom.alarms[alarmIndex].hour / 10);
  lcd.print(eeprom.alarms[alarmIndex].hour % 10);
  lcd.print(':');
  lcd.print(eeprom.alarms[alarmIndex].minute / 10);
  lcd.print(eeprom.alarms[alarmIndex].minute % 10);
  lcd.print("  ");

  // Display alarm duration or state based on program type
  if (eeprom.programType == 0) {
    lcd.print(eeprom.alarms[alarmIndex].duration / 10);
    lcd.print(eeprom.alarms[alarmIndex].duration % 10);
    lcd.print('s');
  } else {
    lcd.print(AlarmState[bitRead(eeprom.alarms[alarmIndex].duration, 0)]);
  }

  // Display days of week for which the alarm is active
  lcd.setCursor(0, 1);
  for (int i = 6; i >= 1; i--) {
    if (bitRead(eeprom.alarms[alarmIndex].days, i)) {
      lcd.print(DaysOfWeek[7 - i][0]);
    } else {
      lcd.print('_');
    }
  }
  lcd.print(bitRead(eeprom.alarms[alarmIndex].days, 7) ? 
        DaysOfWeek[0][0] : '_');

  // Display ON/OFF indicator for alarm enable/disable
  lcd.print("  ");
  lcd.write(bitRead(eeprom.alarms[alarmIndex].days, 0) ? ON_CHAR : OFF_CHAR);

  // Display navigation arrows for alarm selection
  if (alarmIndex == 0) {
    lcd.setCursor(15, 1);
    lcd.write(NEXT_CHAR);
  } else if (alarmIndex == eeprom.alarmCount - 1) {
    lcd.setCursor(13, 1);
    lcd.write(PREV_CHAR);
  } else {
    lcd.setCursor(13, 1);
    lcd.write(PREV_CHAR);
    lcd.setCursor(15, 1);
    lcd.write(NEXT_CHAR);
  }
}
