#pragma once


int sboard_getTemper(void);
void sboard_setTemper(uint16_t _temper);
int sboard_getTemperDst(void);

int sboard_getPressure(void);
int sboard_setLed(uint8_t en);
int sboard_setFan1(uint8_t en);
int sboard_setFan2(uint8_t en);
int sboard_setFan(uint8_t en);
int sboard_getStatus(void);
int sboard_setRetarget(int r);
void sboard_setLock(uint8_t lock);
uint8_t sboard_getLock(void);
