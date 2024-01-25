/*
 * DS3231.h
 *
 *  Created on: Mar 15, 2023
 *      Author: Tuan
 */

#ifndef DS3231_H_
#define DS3231_H_

#include "main.h"
#include "stm32f1xx_hal.h"


// address
#define DS3231_ADDR 0x68<<1

//struct
typedef struct
{
	I2C_HandleTypeDef* I2C;
	uint8_t Sec;
	uint8_t Min;
	uint8_t Hour;
	uint8_t Date;
	uint8_t Day;
	uint8_t Month;
	uint8_t Year;
	uint8_t TxTimeBuff[3];
	uint8_t RxTimeBuff[3];
	uint8_t	TxDateBuff[4];
	uint8_t RxDateBuff[4];
}DS3231_Time;


void DS3231_Init(DS3231_Time* DS3231, I2C_HandleTypeDef* I2C_In);
void DS3231_SetTime(DS3231_Time* DS3231, uint8_t Hour, uint8_t Min, uint8_t Sec);
void DS3231_GetTime(DS3231_Time* DS3231);
void DS3231_SetDate(DS3231_Time* DS3231, uint8_t Day, uint8_t Date, uint8_t Month, uint8_t Year);
void DS3231_GetDate(DS3231_Time* DS3231);


#endif /* DS3231_H_ */
