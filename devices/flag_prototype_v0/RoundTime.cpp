#include "arduinoAll.h"
#include "RoundTime.h"

uint32_t ROUNDMINUTES = ROUND_TIME_30M; 

uint32_t roundDuration;
uint32_t roundTimer;


void InitRound() {
  roundTimer = millis();
  roundDuration = ROUNDMINUTES * 60000;
}

void SetRoundMinutes(uint32_t t) {
  ROUNDMINUTES = t;
}

bool IsRoundEnd() {
  return (millis() - roundTimer >= roundDuration);
}