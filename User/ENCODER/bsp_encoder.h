#ifndef __BSP_ENCODER_H
#define __BSP_ENCODER_H
#include "main.h"
#include "tim.h"
#include "app.h"

uint16_t Encoder_GetLeftCnt(void);
int16_t Encoder_GetRightSpeed(volatile int16_t* encoder_cn);

#endif
