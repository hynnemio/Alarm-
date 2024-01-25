/*
 * DHT11.h
 *
 *  Created on: Mar 17, 2023
 *      Author: Tuan
 */

#ifndef DHT11_H_
#define DHT11_H_

#include "main.h"

typedef struct
{
    TIM_HandleTypeDef *htim;
    GPIO_TypeDef *GPIO_DHT11;
    uint16_t GPIO_Pin_DHT11;
    uint16_t start;
    float Hum;
    float Temp;
} DHT11_HandleTypeDef;

void DHT11_Init(DHT11_HandleTypeDef *DHT, TIM_HandleTypeDef *htim, GPIO_TypeDef *GPIO_DHT11, uint16_t GPIO_Pin_DHT11);

void DHT_Read_Temperature_Humidity(DHT11_HandleTypeDef *DHT);

#endif /* DHT11_H_ */

