#ifndef MEMORY_H
#define MEMORY_H
#include <Arduino.h>

// Program type address
#define PROGRAM_TYPE_ADR 0

// Number of alarms address
#define NBR_ALARMS_ADR (PROGRAM_TYPE_ADR + 1)

// Alarms address
#define ALARMS_ADR (NBR_ALARMS_ADR + 1)

// Password address
#define PASSWORD_ADR (ALARMS_ADR + MAX_ALARMS * 4)

// Program name length address
#define DESCRIPTION_LEN_ADR (PASSWORD_ADR + PASSWORD_LEN)

// Description address
#define DESCRIPTION_ADR (DESCRIPTION_LEN_ADR + 1)

// Author name length address
#define AUTHOR_LEN_ADR (DESCRIPTION_ADR + MAX_DESCRIPTION_LEN)

// Author name address
#define AUTHOR_ADR (AUTHOR_LEN_ADR + 1)

#define EEPROM_OFFSET (AUTHOR_ADR + MAX_AUTHOR_LEN)

// State address
#define STATE_ADR (EEPROM_OFFSET * 3)

// Memory usage calculation:
// MAX_ALARMS * 4 bytes = 160 bytes
// PASSWORD_LEN             = 32 bytes
// MAX_DESCRIPTION_LEN      = 100 bytes
// MAX_AUTHOR_LEN           = 45 bytes
// Length fields (4 x 1B)   = 4 bytes
// Total                    = 341 bytes
// 341 * 3 blocks           = 1023 bytes
// + 1 byte                 = 1024 bytes (1 KB)

void store(uint16_t, byte); // store 3 bytes into EEPROM
void storeProgramType();    // store the program type into EEPROM in PROGRAM_TYPE_ADR
void storeAlarms(); // store alarms into EEPROM in ALARMS_ADDR
void storeDescription(); // store the description into EEPROM in DESCRIPTION_ADDR
void storeAuthor();   // store the author's name into EEPROM in AUTHOR_ADR
void storePassword(); // store the password into EEPROM in PASSWORD_ADDR
void storeState();    // store program state into EEPROM in STATE_ADR
byte get(uint16_t); // retrieve a byte from EEPROM
void getProgramType(); // load the program type into the eeprom variable
void getAlarms();   // load the alarms from ALARMS_ADDR into the eeprom variable
void getPassword(); // load the password from PASSWORD_ADDR into the eeprom variable
void getDescription(); // load the description from DESCRIPTION_ADDR into the eeprom variable
void getAuthor();      // load the author's name from AUTHOR_ADR into the eeprom variable
void getState();       // load the state into the eeprom variable

#endif
