/*
 * delay.c
 *
 *  Created on: Mar 17, 2023
 *      Author: Tuan
 */

#include "delay.h"


void DELAY_Time_Init(TIM_HandleTypeDef *htim)
{
    HAL_TIM_Base_Start(htim);
}


void DELAY_Time_Us(TIM_HandleTypeDef *htim, uint16_t time_us)
{
    HAL_TIM_Base_Start(htim);
    htim->Instance->CNT = 0;
    while ((htim->Instance->CNT) < time_us)
        ;
    HAL_TIM_Base_Stop(htim);
}


void DELAY_Time_Ms(TIM_HandleTypeDef *htim, uint16_t time_ms)
{
    while (time_ms--)
    {
        DELAY_Time_Us(htim, 1000);
    }
}
