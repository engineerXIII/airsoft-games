#ifndef ARDUINO_ALL_H
#define ARDUINO_ALL_H
#include <Arduino.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

// Button work
#define BTN_DEB 50      // тай-маут смены состояния, мс
#define BTN_HOLD 600    // тай-маут удержания, мс

// ------ EEPROM ------
extern uint32_t             EEPROM_SCORESECONDS; 
extern uint32_t             EEPROM_ROUNDMINUTES;
extern uint32_t             EEPROM_CAPTURESECONDS;

extern uint32_t ROUNDMINUTES;   
extern uint32_t SCORESECONDS;  
extern uint32_t CAPTURESECONDS; 
#endif ARDUINO_ALL_H