#ifndef EEPROM_SETTINGS_H
#define EEPROM_SETTINGS_H
#include "arduinoAll.h"
#include "RoundTime.h"

uint32_t EEPROMReadUint32(long address);   
void EEPROMWriteUint32(long address, uint32_t number);  
long EEPROMReadlong(long address);                          
void writeIntIntoEEPROM(int address, int number);           
int readIntFromEEPROM(int address);                         
void readSettingsEEPROM();                                  
void writeSettingsEEPROM();                                 
void EEPROMWritelong(int address, long value);              
void GameSaveHistory(long redTime, long greenTime);   
void GameShowHistory();                               
void writeStringToEEPROM(int addrOffset, const String &strToWrite);
String readStringFromEEPROM(int addrOffset);  

#endif EEPROM_SETTINGS_H