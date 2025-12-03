#include "database.h"
#include "global_vars.h"
#include <Arduino.h>
#include <EEPROM.h>

// -----------------------------------------------------------------------------
// Read a byte from EEPROM using triple redundancy and majority vote
// addr: base EEPROM address
// -----------------------------------------------------------------------------
byte get(uint16_t addr) {
  byte result = 0;
  byte b1 = EEPROM.read(addr);
  byte b2 = EEPROM.read(addr + EEPROM_OFFSET);
  byte b3 = EEPROM.read(addr + EEPROM_OFFSET * 2);

  for (byte i = 0; i < 8; i++) {
    byte count = 0;
    count += bitRead(b1, i);
    count += bitRead(b2, i);
    count += bitRead(b3, i);
    if (count > 1) {
      bitSet(result, i);
    } else {
      bitClear(result, i);
    }
  }
  return result;
}

// -----------------------------------------------------------------------------
// Store a byte to EEPROM using triple redundancy
// -----------------------------------------------------------------------------
void store(uint16_t addr, byte value) {
  for (byte i = 0; i < 3; i++) {
    EEPROM.update(addr, value);
    addr += EEPROM_OFFSET;
  }
}

// -----------------------------------------------------------------------------
// Store all alarms to EEPROM
// -----------------------------------------------------------------------------
void storeAlarms() {
  store(NBR_ALARMS_ADR, eeprom.alarmCount);
  for (byte i = 0; i < eeprom.alarmCount; i++) {
    store(ALARMS_ADR + 4 * i, eeprom.alarms[i].hour);
    store(ALARMS_ADR + 4 * i + 1, eeprom.alarms[i].minute);
    store(ALARMS_ADR + 4 * i + 2, eeprom.alarms[i].duration);
    store(ALARMS_ADR + 4 * i + 3, eeprom.alarms[i].days);
  }
}

// -----------------------------------------------------------------------------
// Load all alarms from EEPROM
// -----------------------------------------------------------------------------
void getAlarms() {
  eeprom.alarmCount = get(NBR_ALARMS_ADR);
  if (eeprom.alarmCount > MAX_ALARMS) eeprom.alarmCount = 0;

  for (byte i = 0; i < eeprom.alarmCount; i++) {
    eeprom.alarms[i].hour = get(ALARMS_ADR + 4 * i);
    eeprom.alarms[i].minute = get(ALARMS_ADR + 4 * i + 1);
    eeprom.alarms[i].duration = get(ALARMS_ADR + 4 * i + 2);
    eeprom.alarms[i].days = get(ALARMS_ADR + 4 * i + 3);
  }
}

// -----------------------------------------------------------------------------
// Store password to EEPROM
// -----------------------------------------------------------------------------
void storePassword() {
  for (byte i = 0; i < PASSWORD_LEN; i++) {
    store(PASSWORD_ADR + i, eeprom.password[i]);
  }
}

// -----------------------------------------------------------------------------
// Load password from EEPROM
// -----------------------------------------------------------------------------
void getPassword() {
  for (byte i = 0; i < PASSWORD_LEN; i++) {
    eeprom.password[i] = get(PASSWORD_ADR + i);
  }
}

// -----------------------------------------------------------------------------
// Store program description to EEPROM
// -----------------------------------------------------------------------------
void storeDescription() {
  store(DESCRIPTION_LEN_ADR, eeprom.descriptionLength);
  for (byte i = 0; i < eeprom.descriptionLength; i++) {
    store(DESCRIPTION_ADR + i, eeprom.description[i]);
  }
}

// -----------------------------------------------------------------------------
// Load program description from EEPROM
// -----------------------------------------------------------------------------
void getDescription() {
  eeprom.descriptionLength = get(DESCRIPTION_LEN_ADR);
  if (eeprom.descriptionLength > MAX_DESCRIPTION_LEN) eeprom.descriptionLength = 0;

  for (byte i = 0; i < eeprom.descriptionLength; i++) {
    eeprom.description[i] = get(DESCRIPTION_ADR + i);
  }
}

// -----------------------------------------------------------------------------
// Store author name to EEPROM
// -----------------------------------------------------------------------------
void storeAuthor() {
  store(AUTHOR_LEN_ADR, eeprom.authorLength);
  for (byte i = 0; i < eeprom.authorLength; i++) {
    store(AUTHOR_ADR + i, eeprom.author[i]);
  }
}

// -----------------------------------------------------------------------------
// Load author name from EEPROM
// -----------------------------------------------------------------------------
void getAuthor() {
  eeprom.authorLength = get(AUTHOR_LEN_ADR);
  if (eeprom.authorLength > MAX_AUTHOR_LEN) eeprom.authorLength = 0;

  for (byte i = 0; i < eeprom.authorLength; i++) {
    eeprom.author[i] = get(AUTHOR_ADR + i);
  }
}

// -----------------------------------------------------------------------------
// Store system state to EEPROM
// -----------------------------------------------------------------------------
void storeState() {
  if (eeprom.state)
    store(STATE_ADR, 0xFE); // ON
  else
    store(STATE_ADR, 0x00); // OFF
}

// -----------------------------------------------------------------------------
// Load system state from EEPROM
// -----------------------------------------------------------------------------
void getState() {
  byte count = 0, b = EEPROM.read(STATE_ADR);
  for (byte j = 7; j > 0; j--) {
    count += bitRead(b, j);
  }
  eeprom.state = (count > 3); // Majority voting
}

// -----------------------------------------------------------------------------
// Store program type to EEPROM
// -----------------------------------------------------------------------------
void storeProgramType() {
  store(PROGRAM_TYPE_ADR, eeprom.programType);
}

// -----------------------------------------------------------------------------
// Load program type from EEPROM
// -----------------------------------------------------------------------------
void getProgramType() {
  eeprom.programType = get(PROGRAM_TYPE_ADR);
}
