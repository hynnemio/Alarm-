/*
 * DHT11.c
 *
 *  Created on: Mar 17, 2023
 *      Author: Tuan
 */

#include "dht11.h"
#include "delay.h"

#define START_TIME 18000 // us
#define TIME_OUT 2000    // ms

static void DHT11_Delay_Init(DHT11_HandleTypeDef *DHT)
{
    DELAY_Time_Init(DHT->htim);
}

static void DHT11_Delay_Us(DHT11_HandleTypeDef *DHT, uint16_t time_us)
{
    DELAY_Time_Us(DHT->htim, time_us);
}

static void Set_Pin_Out(DHT11_HandleTypeDef *DHT)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT->GPIO_Pin_DHT11;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT->GPIO_DHT11, &GPIO_InitStruct);
}

static void Set_Pin_In(DHT11_HandleTypeDef *DHT)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT->GPIO_Pin_DHT11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT->GPIO_DHT11, &GPIO_InitStruct);
}

static uint8_t DHT_ReadPin(DHT11_HandleTypeDef *DHT)
{
    uint8_t value;
    value = HAL_GPIO_ReadPin(DHT->GPIO_DHT11, DHT->GPIO_Pin_DHT11);
    return value;
}

static void DHT_WtritePin(DHT11_HandleTypeDef *DHT, uint8_t state)
{
    HAL_GPIO_WritePin(DHT->GPIO_DHT11, DHT->GPIO_Pin_DHT11, state);
}

static void DHT11_Start(DHT11_HandleTypeDef *DHT)
{
    Set_Pin_Out(DHT);                // set the pin output
    DHT_WtritePin(DHT, 0);           // pull pin the low
    DHT11_Delay_Us(DHT, DHT->start); // waiting for start time
    Set_Pin_In(DHT);                 // set the pin input
}

static uint8_t DHT_Check_Response(DHT11_HandleTypeDef *DHT)
{
    uint8_t response = 0;
    uint32_t timer = 0;
    DHT11_Start(DHT);
    DHT11_Delay_Us(DHT, 40); // waiting for 40 us
    if (!DHT_ReadPin(DHT))
    {
        DHT11_Delay_Us(DHT, 80);
        if (DHT_ReadPin(DHT))
        {
            response = 1;
        }
        else
        {
            response = -1;
        }
    }

    while (DHT_ReadPin(DHT))    // waiting for the pin to go low
    {
        if ((HAL_GetTick() - timer) > TIME_OUT)
        {
            return 0;
        }
    }

    return response;
}

static uint8_t DHT_Read(DHT11_HandleTypeDef *DHT)
{
    uint8_t value = 0;
    uint32_t timer = 0;

    Set_Pin_In(DHT); // set the pin input

    for (int i = 0; i < 8; i++)
    {
        while (!DHT_ReadPin(DHT)) // waiting for the pin to go high
        {
            if ((HAL_GetTick() - timer) > TIME_OUT)
            {
                return 0;
            }
        }

        DHT11_Delay_Us(DHT, 40); // waiting for 40 us

        if (!DHT_ReadPin(DHT))
        {
            value &= ~(1 << (7 - i)); // write 0
        }
        else
        {
            value |= 1 << (7 - i); // write 1
        }

        while (DHT_ReadPin(DHT)) // waiting for the pin to go low
        {
            if ((HAL_GetTick() - timer) > TIME_OUT)
            {
                return 0;
            }
        }
    }
    return value;
}

/**
 * It initializes the DHT11_HandleTypeDef structure with the start time, GPIO port, GPIO pin, and timer
 * handle
 *
 * @param DHT pointer to the DHT11_HandleTypeDef structure
 * @param htim TIM_HandleTypeDef *htim;
 * @param GPIO_DHT11 The GPIO port that the DHT11 is connected to.
 * @param GPIO_Pin_DHT11 The pin that the DHT11 is connected to.
 */
void DHT11_Init(DHT11_HandleTypeDef *DHT, TIM_HandleTypeDef *htim, GPIO_TypeDef *GPIO_DHT11, uint16_t GPIO_Pin_DHT11)
{
    DHT->start = START_TIME;
    DHT->GPIO_DHT11 = GPIO_DHT11;
    DHT->GPIO_Pin_DHT11 = GPIO_Pin_DHT11;
    DHT->htim = htim;
    DHT11_Delay_Init(DHT);
}

/**
 * The function starts by sending a start signal to the DHT11 sensor. Then it reads the data from the
 * sensor and stores it in the appropriate variables. Finally, it converts the data to the appropriate
 * format and stores it in the appropriate variables
 *
 * @param DHT Pointer to the DHT11_HandleTypeDef structure that contains the configuration information
 * for the DHT11.
 */
void DHT_Read_Temperature_Humidity(DHT11_HandleTypeDef *DHT)
{
    uint8_t temp1 = 0, temp2 = 0, RH1 = 0, RH2 = 0;
    uint16_t SUM = 0;
    float temp, humi;
    DHT_Check_Response(DHT);
    RH1 = DHT_Read(DHT);   // Read natural part of humidity
    RH2 = DHT_Read(DHT);   // Read decimal part of humidity
    temp1 = DHT_Read(DHT); // Read natural part of temperature
    temp2 = DHT_Read(DHT); // Read decimal part of temperature
    SUM = DHT_Read(DHT);   // Read sum of the previous 4 bytes

    if ((RH1 + RH2 + temp1 + temp2) == SUM)
    {
        temp = (float)temp1 + (float)(temp2 / 10.0);
        humi = (float)RH1 + (float)(RH2 / 10.0);
        DHT->Temp = temp;
        DHT->Hum = humi;
    }
}
