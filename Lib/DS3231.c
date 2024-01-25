/*
 * DS3231.c
 *
 *  Created on: Mar 15, 2023
 *      Author: Tuan
 */
#include "DS3231.h"

/********************** base layer ********************************/

static void I2C_Write_Time(DS3231_Time* DS3231)
{
	HAL_I2C_Mem_Write(DS3231->I2C, DS3231_ADDR, 0, I2C_MEMADD_SIZE_8BIT, DS3231->TxTimeBuff, 3, 1000);
}

static void I2C_Read_Time(DS3231_Time* DS3231)
{
	HAL_I2C_Mem_Read(DS3231->I2C, DS3231_ADDR, 0, I2C_MEMADD_SIZE_8BIT, DS3231->RxTimeBuff, 3, 1000);
}

static void I2C_Write_Date(DS3231_Time* DS3231)
{
	HAL_I2C_Mem_Write(DS3231->I2C, DS3231_ADDR, 3, I2C_MEMADD_SIZE_8BIT, DS3231->TxDateBuff, 4, 1000);
}

static void I2C_Read_Date(DS3231_Time* DS3231)
{
	HAL_I2C_Mem_Read(DS3231->I2C, DS3231_ADDR, 3, I2C_MEMADD_SIZE_8BIT, DS3231->RxDateBuff, 4,1000);
}

static uint8_t BCD_DEC (uint8_t data)
{
	return (data>>4) * 10 + (data & 0x0F);
}

static uint8_t DEC_BCD (uint8_t data)
{
	return (data/10) << 4 | (data%10);
}


/************************************Main Layer****************************/

void DS3231_Init(DS3231_Time* DS3231, I2C_HandleTypeDef* I2C_In)
{
	DS3231->I2C = I2C_In;
}

void DS3231_SetTime(DS3231_Time* DS3231, uint8_t Hour, uint8_t Min, uint8_t Sec)
{
	DS3231->TxTimeBuff[0] = DEC_BCD(Hour);
	DS3231->TxTimeBuff[1] = DEC_BCD(Min);
	DS3231->TxTimeBuff[2] = DEC_BCD(Sec);
	I2C_Write_Time(DS3231);
}

void DS3231_GetTime(DS3231_Time* DS3231)
{
	I2C_Read_Time(DS3231);
	DS3231->Sec = BCD_DEC(DS3231->RxTimeBuff[0]);
	DS3231->Min = BCD_DEC(DS3231->RxTimeBuff[1]);
	DS3231->Hour = BCD_DEC(DS3231->RxTimeBuff[2]);
}

void DS3231_SetDate(DS3231_Time* DS3231, uint8_t Day, uint8_t Date, uint8_t Month, uint8_t Year)
{
	DS3231->TxDateBuff[0] = DEC_BCD(Day);
	DS3231->TxDateBuff[1] = DEC_BCD(Date);
	DS3231->TxDateBuff[2] = DEC_BCD(Month);
	DS3231->TxDateBuff[3] = DEC_BCD(Year);
	I2C_Write_Date(DS3231);
}

void DS3231_GetDate(DS3231_Time* DS3231)
{
	I2C_Read_Date(DS3231);
	DS3231->Day = BCD_DEC(DS3231->RxDateBuff[0]);
	DS3231->Date = BCD_DEC(DS3231->RxDateBuff[1]);
	DS3231->Month = BCD_DEC(DS3231->RxDateBuff[2]);
	DS3231->Year = BCD_DEC(DS3231->RxDateBuff[3]);
}
