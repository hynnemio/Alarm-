/*
 * delay.h
 *
 *  Created on: Mar 17, 2023
 *      Author: Tuan
 */

#ifndef DELAY_H_
#define DELAY_H_

#include "main.h"

void DELAY_Time_Init(TIM_HandleTypeDef *htim);

void DELAY_Time_Us(TIM_HandleTypeDef *htim, uint16_t time_us);

void DELAY_Time_Ms(TIM_HandleTypeDef *htim, uint16_t time_ms);


#endif /* DELAY_H_ */
