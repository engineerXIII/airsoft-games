#ifndef ROUND_TIME_H
#define ROUND_TIME_H

#define ROUND_TIME_1H 60         // 1h, тай-маут игры,минмс
#define ROUND_TIME_30M 30        // 30m, тай-маут игры, мин
#define ROUND_TIME_1H_MS 3600000         // 1h, тай-маут игры, мс
#define ROUND_TIME_30M_MS 1800000        // 30m, тай-маут игры, мс

void InitRound();

void SetRoundMinutes(uint32_t t);

bool IsRoundEnd();

#endif ROUND_TIME_H
