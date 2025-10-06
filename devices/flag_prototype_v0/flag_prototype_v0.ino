#include "FastUtils.h"
#define _LCD_TYPE 1
#include <LCD_1602_RUS_ALL.h>
#include <font_LCD_1602_RUS.h>
LCD_1602_RUS lcd(0x3F, 16, 2);

//#include <LiquidCrystal_I2C.h>
//LiquidCrystal_I2C lcd(0x3F, 16, 2);
/////////////////////////////////////
// Language settings
/////////////////////////////////////
#define LOC_RU 1
#define LOC_EN 2
#define LOCALE LOC_RU
// Messages LED screen
#if LOCALE == LOC_RU
#define MSG_TEAM_IS "КОМАНДА "
#define MSG_FLAG_IN_PROGRESS_CAPTURE "ЗАХВАТ ФЛАГА"
#define MSG_FLAG_CAPTURED "ФЛАГ ЗАХВАЧЕН"
#define MSG_END_OF_GAME "КОНЕЦ ИГРЫ"
#elif LOCALE == LOC_EN
#define MSG_TEAM_IS "Team is "
#define MSG_FLAG_IN_PROGRESS_CAPTURE "Flag capturing"
#define MSG_FLAG_CAPTURED "Flag captured"
#define MSG_END_OF_GAME "End of game"
#endif

/////////////////////////////////////
// Hardware settings
/////////////////////////////////////
#define RED_BUTTON_PIN 10
#define GREEN_BUTTON_PIN 11
#define BLUE_BUTTON_PIN 12

// Button work
#define BTN_DEB 50      // тай-маут смены состояния, мс
#define BTN_HOLD 500    // тай-маут удержания, мс

uint32_t btnTm;
bool needTmUpdate = false;
bool bRedState = false;
bool bGreenState = false;
bool bBlueState = false;
bool bRedHold = false;
bool bGreenHold = false;
bool bBlueHold = false;

// LED Indication
#define RED_LED_PIN 3
#define GREEN_LED_PIN 6
#define BLUE_LED_PIN 7

#define BLINK_TIME_MS 500

/*
 3210
 ||||
 0000
 - 
 - 1 - RED
 - 2 - GREEN
 - 3 - BLUE
*/
#define RED_BIT 1
#define GREEN_BIT 2
#define BLUE_BIT 3
uint8_t blinkStatus;
#define GET_BLINK_STATUS(blinkStatus, led) (blinkStatus & (1 << led))
#define SET_BLINK_STATUS(blinkStatus, led) (blinkStatus |= (1 << led))
#define RESET_BLINK_STATUS(blinkStatus, led) (blinkStatus &= ~(1 << led))

uint32_t blinkTm;

/////////////////////////////////////
// Game settings
/////////////////////////////////////
// Teams
#define NO_TEAM 255
#define RED_TEAM 0
#define GREEN_TEAM 1
#define BLUE_TEAM 2

// Game state
#define EMPTY 0
#define CAPTURED 1
#define RESET_CAPTURE 2
#define SET_CAPTURE 3
#define END_GAME 4
// Timers
#define CAPTURE_CHANGE_TIME 10000      // тай-маут смены состояния, мс
#define CAPTURE_SCORE 15000      // тай-маут смены состояния, мс
#define ROUND_TIME 3600000
#define SCORE_SHOW_TIME 3000


uint32_t stateChangeTm;
uint32_t stateTm;
uint8_t prevTeam;
uint8_t currTeam;
uint8_t state;
uint32_t roundTm;

uint16_t teamRedScore;
uint16_t teamGreenScore;
uint16_t teamBlueScore;

struct TeamCfg {
  uint8_t Id;
  uint8_t Score;
  uint8_t led_pin;
  uint8_t led_blink_bit;
  uint8_t button_pin;
};

TeamCfg teams[3];

void setup() {
    Serial.begin(115200);

    lcd.init();           // инициализация
    lcd.backlight();      // включить подсветку
    lcd.clear();   

    pinMode(RED_BUTTON_PIN, INPUT_PULLUP);
    pinMode(GREEN_BUTTON_PIN, INPUT_PULLUP);
    pinMode(BLUE_BUTTON_PIN, INPUT_PULLUP);
    
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);

    // Reset LEDs
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(BLUE_LED_PIN, LOW);

    
    digitalWrite(RED_LED_PIN, HIGH);
    delay(500);
    digitalWrite(RED_LED_PIN, LOW);
    delay(500);
    digitalWrite(GREEN_LED_PIN, HIGH);
    delay(500);
    digitalWrite(GREEN_LED_PIN, LOW);
    delay(500);
    digitalWrite(BLUE_LED_PIN, HIGH);
    delay(500);
    digitalWrite(BLUE_LED_PIN, LOW);
    delay(500);
    
    blinkStatus = 0;
    blinkTm = millis();
    btnTm = millis();
    needTmUpdate = false;

    // Reset game state
    stateChangeTm = millis();
    stateTm = millis();
    roundTm = millis();
    state = EMPTY;
    currTeam = NO_TEAM;
    prevTeam = NO_TEAM;
    teamRedScore = EMPTY;
    teamGreenScore = EMPTY;
    teamBlueScore = EMPTY;

    teams[RED_TEAM].Id = RED_TEAM;
    teams[RED_TEAM].Score = EMPTY;
}

void checkButton(uint8_t buttonPin, uint8_t ledPin, bool *pState, bool *hold) {
    bool state = !digitalRead(buttonPin);
    if (*pState != state && millis() - btnTm >= BTN_DEB) {
        needTmUpdate = true;
        *pState = state;
        *hold = false;   // сброс флага удержания
        if (state) Serial.println("Кнопка нажата " + buttonPin);
        else  {
          Serial.println("Кнопка отпущена " + buttonPin);
          digitalWrite(ledPin, LOW);
        }
    }

    // кнопка удерживается дольше 500 мс
    if (*pState && !*hold && millis() - btnTm >= BTN_HOLD) {
        *hold = true;    // флаг удержания
        Serial.println("Кнопка удержана");
    }


}



const char* teamIdToString(uint8_t id) {
  switch (id) {
    case RED_TEAM:
      return "  RED";
    case GREEN_TEAM:
      return "GREEN";
    case BLUE_TEAM:
      return " BLUE";
  }
}

uint8_t getTeamLedBit(uint8_t team) {
  switch (team) {
    case RED_TEAM:
      return RED_BIT;
    case GREEN_TEAM:
      return GREEN_BIT;
    case BLUE_TEAM:
      return BLUE_BIT;
  }
}

uint8_t getTeamLedPin(uint8_t team) {
  switch (team) {
    case RED_TEAM:
      return RED_LED_PIN;
    case GREEN_TEAM:
      return GREEN_LED_PIN;
    case BLUE_TEAM:
      return BLUE_LED_PIN;
  }
}

uint8_t getTeamBtnPressed() {
  if (bRedState) {
    return RED_TEAM;
  } else if (bGreenState) {
    return GREEN_TEAM;
  } else if (bBlueState) {
    return BLUE_TEAM;
  }
}

void loop() {
    // Detect end of round
    if (millis() - roundTm >= ROUND_TIME) {
        lcd.home();
        lcd.clear();
        lcd.print(MSG_END_OF_GAME);
        delay(SCORE_SHOW_TIME);
        while(true) {
          // int teamId = RED_TEAЬ
          // lcd.setCursor(0, 1);
          // lcd.print("Team ");
          // lcd.print(teamIdToString(teamId));
          // lcd.print(":              ");
          // lcd.setCursor(12, 1);
          // lcd.print(teamRedScore);
          // teamId++;
          // delay(SCORE_SHOW_TIME);
          
          lcd.setCursor(0, 1);
          lcd.print("Team   RED:              ");
          lcd.setCursor(12, 1);
          lcd.print(teamRedScore, DEC);
          delay(SCORE_SHOW_TIME);
          
          lcd.setCursor(0, 1);
          lcd.print("Team GREEN:              ");
          lcd.setCursor(12, 1);
          lcd.print(teamGreenScore, DEC);
          delay(SCORE_SHOW_TIME);
          lcd.setCursor(0, 1);
          lcd.print("Team  BLUE:              ");
          lcd.setCursor(12, 1);
          lcd.print(teamBlueScore, DEC);
          delay(SCORE_SHOW_TIME);
        }
        return;
    }

    // Led blink handle
    if (millis() - blinkTm >= BLINK_TIME_MS) {
        blinkTm += BLINK_TIME_MS;
        if (GET_BLINK_STATUS(blinkStatus, RED_BIT)) {
            digitalToggleFast(RED_LED_PIN);
        }
        if (GET_BLINK_STATUS(blinkStatus, GREEN_BIT)) {
            digitalToggleFast(GREEN_LED_PIN);
        }
        if (GET_BLINK_STATUS(blinkStatus, BLUE_BIT)) {
            digitalToggleFast(BLUE_LED_PIN);
        }
    }

    checkButton(RED_BUTTON_PIN, RED_LED_PIN, &bRedState, &bRedHold);
    checkButton(GREEN_BUTTON_PIN, GREEN_LED_PIN, &bGreenState, &bGreenHold);
    checkButton(BLUE_BUTTON_PIN, BLUE_LED_PIN, &bBlueState, &bBlueHold);
    if (needTmUpdate) {
      btnTm = millis();
      needTmUpdate = false;
    }

    switch(state) {
      case EMPTY:  // we need if somebody pressed button to start capture
        lcd.home();
        lcd.clear();   
        if (bRedHold && !bGreenHold && !bBlueHold)
        {
          state = SET_CAPTURE;
          currTeam = RED_TEAM;
          SET_BLINK_STATUS(blinkStatus, RED_BIT);
        lcd.print(MSG_FLAG_IN_PROGRESS_CAPTURE);
        } else if (!bRedHold && bGreenHold && !bBlueHold)
        {
          state = SET_CAPTURE;
          currTeam = GREEN_TEAM;
          SET_BLINK_STATUS(blinkStatus, GREEN_BIT);
          lcd.print(MSG_FLAG_IN_PROGRESS_CAPTURE);
        } else if (!bRedHold && !bGreenHold && bBlueHold)
        {
          state = SET_CAPTURE;
          currTeam = BLUE_TEAM;
          SET_BLINK_STATUS(blinkStatus, BLUE_BIT);
          lcd.print(MSG_FLAG_IN_PROGRESS_CAPTURE);
        } // else blink error led TODO
        break;
      case SET_CAPTURE:  // we need to check if button is still captured
        switch (currTeam) {
          case RED_TEAM:
            if (!bRedState) {
              RESET_BLINK_STATUS(blinkStatus, RED_BIT);
              state = EMPTY;
              stateChangeTm = millis();
            } else {
              if (millis() - stateChangeTm >= CAPTURE_CHANGE_TIME) {
                stateChangeTm += CAPTURE_CHANGE_TIME;
                RESET_BLINK_STATUS(blinkStatus, RED_BIT);
                digitalWriteFast(RED_LED_PIN, HIGH);
                lcd.home();
                lcd.clear();
                lcd.print(MSG_FLAG_CAPTURED);
                lcd.setCursor(0, 1);  // столбец 0 строка 1
                lcd.print(MSG_TEAM_IS);
                lcd.print(teamIdToString(currTeam));
                state = CAPTURED;
                stateTm = millis();
              }
            }
            break;
          case GREEN_TEAM:
            if (!bGreenState) {
              RESET_BLINK_STATUS(blinkStatus, GREEN_BIT);
              state = EMPTY;
              stateChangeTm = millis();
            } else {
              if (millis() - stateChangeTm >= CAPTURE_CHANGE_TIME) {
                stateChangeTm += CAPTURE_CHANGE_TIME;
                RESET_BLINK_STATUS(blinkStatus, GREEN_BIT);
                digitalWriteFast(GREEN_LED_PIN, HIGH);
                lcd.home();
                lcd.clear();
                lcd.print(MSG_FLAG_CAPTURED);
                lcd.setCursor(0, 1);  // столбец 0 строка 1
                lcd.print(MSG_TEAM_IS);
                lcd.print(teamIdToString(currTeam));
                state = CAPTURED;
                stateTm = millis();
              }
            }
            break;
          case BLUE_TEAM:
            if (!bBlueState) {
              RESET_BLINK_STATUS(blinkStatus, BLUE_BIT);
              state = EMPTY;
              stateChangeTm = millis();
            } else {
              if (millis() - stateChangeTm >= CAPTURE_CHANGE_TIME) {
                stateChangeTm += CAPTURE_CHANGE_TIME;
                RESET_BLINK_STATUS(blinkStatus, BLUE_BIT);
                digitalWriteFast(BLUE_LED_PIN, HIGH);
                lcd.home();
                lcd.clear();
                lcd.print(MSG_FLAG_CAPTURED);
                lcd.setCursor(0, 1);  // столбец 0 строка 1
                lcd.print(MSG_TEAM_IS);
                lcd.print(teamIdToString(currTeam));
                state = CAPTURED;
                stateTm = millis();
              }
            }
            break;
          default:
            state = EMPTY;
            stateChangeTm = millis();
            break;
        }
        break;
      case RESET_CAPTURE: // we need to check if button is still captured
        switch(currTeam) {
          case RED_TEAM:
            if (bRedHold) {
              SET_BLINK_STATUS(blinkStatus, getTeamLedBit(prevTeam));
              if (millis() - stateChangeTm >= CAPTURE_CHANGE_TIME) {
                stateChangeTm += CAPTURE_CHANGE_TIME;
                state = EMPTY;
                RESET_BLINK_STATUS(blinkStatus, getTeamLedBit(prevTeam));
              }
            } else {
              RESET_BLINK_STATUS(blinkStatus, getTeamLedBit(prevTeam));
              currTeam = prevTeam;
              state = CAPTURED;
            }
            break;
          case GREEN_TEAM:
            if (bGreenHold) {
              SET_BLINK_STATUS(blinkStatus, getTeamLedBit(prevTeam));
              if (millis() - stateChangeTm >= CAPTURE_CHANGE_TIME) {
                stateChangeTm += CAPTURE_CHANGE_TIME;
                state = EMPTY;
                RESET_BLINK_STATUS(blinkStatus, getTeamLedBit(prevTeam));
              }
            } else {
              RESET_BLINK_STATUS(blinkStatus, getTeamLedBit(prevTeam));
              currTeam = prevTeam;
              state = CAPTURED;
            }
            break;
          case BLUE_TEAM:
            if (bBlueHold) {
              SET_BLINK_STATUS(blinkStatus, getTeamLedBit(prevTeam));
              if (millis() - stateChangeTm >= CAPTURE_CHANGE_TIME) {
                stateChangeTm += CAPTURE_CHANGE_TIME;
                state = EMPTY;
                RESET_BLINK_STATUS(blinkStatus, getTeamLedBit(prevTeam));
              }
            } else {
              RESET_BLINK_STATUS(blinkStatus, getTeamLedBit(prevTeam));
              currTeam = prevTeam;
              state = CAPTURED;
            }
            break;
        }
        break;
      case CAPTURED:  // we need if somebody pressed button to reset capture
        // we need if somebody pressed button to reset capture
        if ((currTeam == RED_TEAM && (bGreenState || bBlueState)) || 
          (currTeam == GREEN_TEAM && (bRedState || bBlueState)) || 
          (currTeam == BLUE_TEAM && (bGreenState || bRedState)) ) {
          prevTeam = currTeam;
          currTeam = getTeamBtnPressed();
          state = RESET_CAPTURE;
          stateChangeTm = millis();
          lcd.home();
          lcd.clear();
          lcd.print("Flag resetting");
        } else {
          digitalWriteFast(getTeamLedPin(currTeam), HIGH);
          lcd.home();
          lcd.print(MSG_FLAG_CAPTURED);
          lcd.print("  ");
          if (millis() - stateTm >= CAPTURE_SCORE) {
            stateTm += CAPTURE_SCORE;
            switch(currTeam) {
             case RED_TEAM:
                teamRedScore += 1;
                digitalWriteFast(RED_LED_PIN, LOW);
                digitalWriteFast(GREEN_LED_PIN, LOW);
                digitalWriteFast(BLUE_LED_PIN, LOW);
                delay(300);
                digitalWriteFast(RED_LED_PIN, HIGH);
                lcd.setCursor(0, 1);
                lcd.print("Team RED:              ");
                lcd.setCursor(12, 1);
                lcd.print(teamRedScore, DEC);
                break;
              case GREEN_TEAM:
                teamGreenScore += 1;
                digitalWriteFast(RED_LED_PIN, LOW);
                digitalWriteFast(GREEN_LED_PIN, LOW);
                digitalWriteFast(BLUE_LED_PIN, LOW);
                delay(300);
                digitalWriteFast(GREEN_LED_PIN, HIGH);
                lcd.setCursor(0, 1);
                lcd.print("Team GREEN:       ");
                lcd.setCursor(12, 1);
                lcd.print(teamGreenScore, DEC);
                break;
              case BLUE_TEAM:
                teamBlueScore += 1;
                digitalWriteFast(RED_LED_PIN, LOW);
                digitalWriteFast(GREEN_LED_PIN, LOW);
                digitalWriteFast(BLUE_LED_PIN, LOW);
                delay(300);
                digitalWriteFast(BLUE_LED_PIN, HIGH);
                lcd.setCursor(0, 1);
                lcd.print("Team BLUE:       ");
                lcd.setCursor(12, 1);
                lcd.print(teamBlueScore, DEC);
                break;
             }
          }
        }
        break;
    }
}
