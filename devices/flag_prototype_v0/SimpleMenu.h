#include "arduinoAll.h"
#include "RoundTime.h"
extern uint32_t             EEPROM_ROUNDMINUTES;

#define MSG_CHOOSE_TIME "30m         1h  "
#define MSG_CHOOSEN_1   " ^              "
#define MSG_CHOOSEN_2   "            ^   "

void HandleChooseTime(LiquidCrystal_I2C *lcd, uint8_t optBtn1, uint8_t optBtn2) {
  bool pState1 = false;
  bool pState2 = false;
  uint32_t btnTm = millis();
  lcd->setCursor(0, 0);
  lcd->print(MSG_CHOOSE_TIME);
  switch (ROUNDMINUTES) {
    case  ROUND_TIME_30M:
      lcd->setCursor(0, 1);
      lcd->print(MSG_CHOOSEN_1);
      break;
    case  ROUND_TIME_1H:
      lcd->setCursor(0, 1);
      lcd->print(MSG_CHOOSEN_2);
      break;
  }
  while(true) {
    bool state1 = digitalRead(optBtn1);
    bool state2 = digitalRead(optBtn2);
    if (pState1 != state1 && millis() - btnTm >= BTN_DEB) {
        pState1 = state1;
    }
    if (pState1 != state2 && millis() - btnTm >= BTN_DEB) {
        pState2 = state2;
    }

    if (state1 != state2) {
      if (state1) {
        EEPROM_ROUNDMINUTES=ROUND_TIME_30M;
        lcd->setCursor(0, 1);
        lcd->print(MSG_CHOOSEN_1);
        delay(3000);
        break;
      }
      if (state2) {
        EEPROM_ROUNDMINUTES=ROUND_TIME_1H;
        lcd->setCursor(0, 1);
        lcd->print(MSG_CHOOSEN_2);
        delay(3000);
        break;
      }
    }

  }
}