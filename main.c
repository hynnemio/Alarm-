/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.


  * Main Author: VuTuanLinh / BlingMio


  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DS3231.h"
#include "lcd_i2c.h"
#include "delay.h"
#include "stdio.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
	uint8_t device;
	DS3231_Time DS3231;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
LCD_I2C_HandleTypeDef p_lcd;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
	  	enum state{
		DATE,
		SET_HOUR,
		SET_MIN,
		SET_SEC,
		SET_DAY,
		SET_YEAR,
		SET_MONTH,
		SET_DATE,
		   };
       enum alarm{
	    ALARM,
		ALARM_HOUR,
		ALARM_MIN,
		ALARM_SEC,
		ALARM_DAY,
		ALARM_YEAR,
		ALARM_MONTH,
		ALARM_DATE
           };



    enum alarm alarm;
	enum state state;
	char time[15];
	uint16_t savealarmmin;
	uint8_t savealarmhour;
	uint8_t alarmday, alarmdate, alarmmonth, alarmyear;


	uint8_t savehour;
    uint16_t savemin;
    uint16_t savesec;
    uint16_t saveday, savedate, savemonth, saveyear;
    uint32_t timer = 0;


    uint8_t counter = 0;
    uint8_t trangthai = 0;
    uint8_t dem = 0;
    uint32_t flag = 0;


    uint32_t tempCode;
    uint8_t bitIndex;
    uint8_t cmd;
    uint8_t cmdli;
    uint32_t code;


    void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
	{
		counter = __HAL_TIM_GET_COUNTER(htim);
	}

    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
	{


	  if(GPIO_Pin == GPIO_PIN_10)
	  {
		if (__HAL_TIM_GET_COUNTER(&htim1) > 8000 )
		{
		  tempCode = 0;
		  bitIndex = 0;
		}
		else if (__HAL_TIM_GET_COUNTER(&htim1) > 1700)
		{
		  tempCode |= (1UL << (31-bitIndex));
		  bitIndex++;
		}
		else if (__HAL_TIM_GET_COUNTER(&htim1) > 1000)
		{
		  tempCode &= ~(1UL << (31-bitIndex));
		  bitIndex++;
		}
		if(bitIndex == 32)
		{
		  cmdli = ~tempCode;
		  cmd = tempCode >> 8;
		  if(cmdli == cmd)
		  {
			code = tempCode;
		  }
		  bitIndex = 0;
		}
		__HAL_TIM_SET_COUNTER(&htim1, 0);
		flag = code;
	  }


	  if(GPIO_Pin == GPIO_PIN_8 || flag == 16712445)
		  {
	  	  if(HAL_GetTick()- timer < 90)
		  {
			return;
		  }
			timer = HAL_GetTick();
			  if (state == DATE)
			  {
				  lcd_clear(&p_lcd);
				  state++;
				  savehour = DS3231.Hour;
				  savemin = DS3231.Min;
				  savesec = DS3231.Sec;
				  saveday = DS3231.Day;
				  savedate = DS3231.Date;
				  savemonth = DS3231.Month;
				  saveyear = DS3231.Year;
				  flag = 0;
			  }
			  else{
				  state++;
				  flag = 0;
				  if (state - SET_SEC == 1)
					  {
					   state = SET_DAY;
					  }
				  if (state - SET_DATE == 1)
					  {
					   state = DATE;
					   DS3231_SetTime(&DS3231, savehour, savemin, savesec);
					   DS3231_SetDate(&DS3231, saveday, savedate, savemonth, saveyear);
					  }
					}
		  }


	  if(GPIO_Pin == GPIO_PIN_11 || flag == 16769055)
	  	  	  {
	  	  if(HAL_GetTick()- timer < 90)
		  {
			return;
		  }
			timer = HAL_GetTick();
	  	  		  if (alarm == ALARM)
	  			  {
	  				  lcd_clear(&p_lcd);
	  				  alarm = ALARM_DAY;
					  alarmday   = DS3231.Day;
	  				  alarmdate  = DS3231.Date;
	  				  alarmmonth = DS3231.Month;
	  				  alarmyear  = DS3231.Year;
	  				  flag = 0;
	  			  }
	  	  		  else
	  	  		  {
	  				  alarm++;
	  				  if (alarm - ALARM_MIN == 1)
	  				  	  {
	  					  alarm = ALARM;
	  				  	  }
					  if (alarm - ALARM_DATE == 1)
						  {
						   alarm = ALARM_HOUR;
			  			   savealarmhour = DS3231.Hour;
						   savealarmmin  = DS3231.Min;
						  }
	  				 flag = 0;
	  	  		   }
	  	  	      }


	  if (GPIO_Pin == GPIO_PIN_15)
	  	  	  {
					  savealarmhour = 0;
					  savealarmmin   = 0;
					  alarmday   = 0;
					  alarmdate  = 0;
					  alarmmonth = 0;
					  alarmyear  = 0;
	  	  	  }


	  if(GPIO_Pin == GPIO_PIN_9 || flag == 16761405)
	 	 {
	  	  if(HAL_GetTick()- timer < 90)
		  {
			return;
		  }
			timer = HAL_GetTick();
      switch (state)
	 	 {
	 	 	 case SET_SEC:
	 	 		 savesec++;
	 	 		 flag = 0;
	 	 		 if (savesec > 59)

	 	 		 {
	 	 			 savesec = 0;
	 	 		 }
		     break;

			case SET_MIN:
				savemin++;
				flag = 0;
				if (savemin > 59)
					{
					 savemin = 0;
					}
				break;
			case SET_HOUR:
				savehour++;
				flag = 0;
				if (savehour > 23)
					{
					 savehour = 0;
					}
				break;
			case SET_DAY:
				saveday++;
				flag = 0;
				if (saveday > 7 )
					{
					saveday = 1;
					}
				break;
			case SET_YEAR:
				saveyear++;
				flag = 0;
				if ( saveyear > 30)
				{
					saveyear = 24;
				}
				break;
			case SET_MONTH:
				savemonth++;
				flag = 0;
				if ( savemonth > 12)
				{
					savemonth = 1;
				}
				break;
			case SET_DATE:
				savedate++;
				flag = 0;
				if (savemonth == 1
						|| savemonth == 3
						|| savemonth == 5
						|| savemonth == 7
						|| savemonth == 8
						|| savemonth == 10
						|| savemonth == 12 )
					{
					if (savedate > 31)
						{
						savedate = 1;
						}
					}
				if (savemonth == 2 && saveyear == 24)
					{
					if (savedate > 29)
						{
						savedate = 1;
						}
					}
				else if (savemonth == 2 && saveyear == 28)
				    {
					if (savedate > 29)
						{
						savedate = 1;
						}
					}
				else if (savemonth == 2 && saveyear != 24)
					{
					if (savedate > 28)
						{
						savedate = 1;
						}
					}
				else if (savemonth == 2 && saveyear != 28)
					{
					if (savedate > 28)
						{
						savedate = 1;
						}
					}
				if (savemonth == 4
						|| savemonth == 6
						|| savemonth == 3
						|| savemonth == 9
						|| savemonth == 11)
					{
					if (savedate > 30)
						{
						savedate = 1;
						}
					}
				break;
	 	 }
	 	 }


      if(GPIO_Pin == GPIO_PIN_12 || flag == 16754775)
      {
	  	  if(HAL_GetTick()- timer < 90)
		  {
			return;
		  }
			timer = HAL_GetTick();
				switch(alarm)
				{
				case ALARM_MIN:
					savealarmmin++;
					flag = 0;
					if (savealarmmin > 59)
						{
						savealarmmin = 0;
						}
					break;
				case ALARM_HOUR:
					savealarmhour ++;
					flag = 0;
					if (savealarmhour > 23)
						{
						savealarmhour = 0;
						}
					break;
				case ALARM_DAY:
					alarmday++;
					flag = 0;
					if (alarmday > 7 )
						{
						alarmday = 1;
						}
					break;
				case ALARM_YEAR:
					alarmyear++;
					flag = 0;
					if ( alarmyear > 30)
					{
						alarmyear = 24;
					}
					break;
				case ALARM_MONTH:
					alarmmonth++;
					flag = 0;
					if ( alarmmonth > 12)
					{
						alarmmonth = 1;
					}
					break;
				case ALARM_DATE:
					alarmdate++;
					flag = 0;
					if (alarmmonth == 1
							|| alarmmonth == 3
							|| alarmmonth == 5
							|| alarmmonth == 7
							|| alarmmonth == 8
							|| alarmmonth == 10
							|| alarmmonth == 12 )
						{
						if (alarmdate > 31)
							{
							alarmdate = 1;
							}
						}
					if (alarmmonth == 2 && alarmyear == 24)
						{
						if (alarmdate > 29)
							{
							alarmdate = 1;
							}
						}
					else if (alarmmonth == 2 && alarmyear == 28)
						{
						if (alarmdate > 29)
							{
							alarmdate = 1;
							}
						}
					else if (alarmmonth == 2 && alarmyear != 24)
						{
						if (alarmdate > 28)
							{
							alarmdate = 1;
							}
						}
					else if (alarmmonth == 2 && alarmyear != 28)
						{
						if (alarmdate > 28)
							{
							alarmdate = 1;
							}
						}
					if (alarmmonth == 4
							|| alarmmonth == 6
							|| alarmmonth == 3
							|| alarmmonth == 9
							|| alarmmonth == 11)
						{
						if (alarmdate > 30)
							{
							alarmdate = 1;
							}
						}
					break;
		 default:
			    break;
		}

      }

	}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  lcd_i2c_init(&p_lcd, &hi2c1, 20, 4, 0x27<<1);
  HAL_TIM_Base_Start(&htim2);
  HAL_TIM_Base_Start(&htim1);
  __HAL_TIM_SET_COUNTER(&htim1, 0);

  for (int i = 0; i < 255; i++)
  {
	   if (HAL_I2C_IsDeviceReady(&hi2c1, i, 1, 100) == HAL_OK)
	   {
		   device = i;
	   }
  }

  DS3231_Init(&DS3231, &hi2c1);
  HAL_TIM_Encoder_Start_IT(&htim3, TIM_CHANNEL_ALL);
  uint32_t timer1 = HAL_GetTick();
//  DS3231_SetTime(&DS3231, 7, 53, 0);
//  DS3231_SetDate(&DS3231, 5, 25, 1, 24);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


	  DS3231_GetTime(&DS3231);
	  DS3231_GetDate(&DS3231);

	  if (    alarm == ALARM
			  && DS3231.Hour == savealarmhour
			  && DS3231.Min == savealarmmin
			  && DS3231.Day == alarmday
			  && DS3231.Date == alarmdate
			  && DS3231.Month == alarmmonth
			  && DS3231.Year == alarmyear)
		   {HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, SET);}
	  else {HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, RESET);}


	  switch (state)
	  {
	  	 case DATE:
	  		DS3231_GetTime(&DS3231);
		    DS3231_GetDate(&DS3231);
		    HAL_Delay(100);
		    lcd_clear(&p_lcd);
					if (DS3231.Day == 1)
					{
						lcd_set_cursor(&p_lcd, 0, 1);
						sprintf(time, "CN %d-%d-20%d" , DS3231.Date, DS3231.Month, DS3231.Year);
						lcd_send_string(&p_lcd, time);
					}
					else
					{
						lcd_set_cursor(&p_lcd, 0, 1);
						sprintf(time, "Thu %d %d-%d-20%d", DS3231.Day, DS3231.Date, DS3231.Month, DS3231.Year);
						lcd_send_string(&p_lcd, time);
					}

                    if (HAL_GetTick()- timer1 >= 5000)
                    	{
                    	  lcd_clear(&p_lcd);
                    	  lcd_set_cursor(&p_lcd, 0, 1);
                    	  sprintf(time, "%02d:%02d", savealarmhour, savealarmmin);
                    	  lcd_send_string(&p_lcd, time);

                    	  if (alarmday == 1)
						{
							lcd_set_cursor(&p_lcd, 6, 1);
							sprintf(time, "CN/%d/%d/%d" , alarmdate, alarmmonth, alarmyear);
							lcd_send_string(&p_lcd, time);
						}
						else
						{
							lcd_set_cursor(&p_lcd, 6, 1);
							sprintf(time, "%d/%d/%d/%d", alarmday, alarmdate, alarmmonth, alarmyear);
							lcd_send_string(&p_lcd, time);
						}
                    	  lcd_set_cursor(&p_lcd, 4, 0);
						  sprintf(time, "%02d:%02d:%02d ", DS3231.Hour, DS3231.Min, DS3231.Sec);
						  lcd_send_string(&p_lcd, time);
					    }
					  if (HAL_GetTick()- timer1 >= 8500)
					  {
					  timer1 = HAL_GetTick();
					  }


		    lcd_set_cursor(&p_lcd, 4, 0);
			sprintf(time, "%02d:%02d:%02d ", DS3231.Hour, DS3231.Min, DS3231.Sec);
			lcd_send_string(&p_lcd, time);
	  		break;

	  	 case SET_HOUR:
	  		if (trangthai < counter)
					{
					  trangthai = counter;
					  savehour++;
							if (savehour > 23)
								{
								savehour = 0;
								}
						trangthai = counter;
					}
			  else if (trangthai > counter)
					{
						trangthai = counter;
						savehour--;
						if (savehour < 0)
							{
							savehour = 23;
							}
						trangthai = counter;
					}
			  if (trangthai = counter)
					{
					  if (savehour > 23)
						{
						  savehour = 23;
						}
					}
	  		lcd_clear(&p_lcd);
	  		lcd_set_cursor(&p_lcd, 4, 0);
	  		sprintf(time, " +%02d:%02d:%02d", savehour, savemin, savesec);
	  		lcd_send_string(&p_lcd, time);
	  		HAL_Delay(150);
	  	 	break;

	  	 case SET_MIN:
	  		if (trangthai < counter)
					{
					  trangthai = counter;
					  savemin++;
							if (savemin > 59)
								{
								savemin = 0;
								}
						trangthai = counter;
					}
			  else if (trangthai > counter)
					{
						trangthai = counter;
						savemin--;
						if (savemin < 0)
							{
							savemin = 59;
							}
						trangthai = counter;
					}
			  if (trangthai = counter)
					{
					  if (savemin > 59)
						{
						  savemin = 59;
						}
					}
		 	lcd_clear(&p_lcd);
	  		lcd_set_cursor(&p_lcd, 4, 0);
	  		sprintf(time, "  %02d:+%02d:%02d", savehour, savemin, savesec);
			lcd_send_string(&p_lcd, time);
			HAL_Delay(150);
		    break;

	  	 case SET_SEC:
	  		if (trangthai < counter)
					{
					  trangthai = counter;
					  savesec++;
							if (savesec > 59)
								{
								savesec = 0;
								}
						trangthai = counter;
					}
			  else if (trangthai > counter)
					{
						trangthai = counter;
						savesec--;
						if (savesec < 0)
							{
							savesec = 59;
							}
						trangthai = counter;
					}
			  if (trangthai = counter)
					{
					  if (savesec > 59)
						{
						  savesec = 59;
						}
					}
		 	lcd_clear(&p_lcd);
			lcd_set_cursor(&p_lcd, 4, 0);
			sprintf(time, " %02d:%02d:+%02d", savehour, savemin, savesec);
			lcd_send_string(&p_lcd, time);
			HAL_Delay(150);
			break;

	  	 case SET_DAY:
	  		if (trangthai < counter)
					{
					  trangthai = counter;
					  saveday++;
							if ( saveday > 7)
								{
								saveday = 1;
								}
						trangthai = counter;
					}
			  else if (trangthai > counter)
					{
						trangthai = counter;
						saveday--;
						if (saveday < 1)
							{
							saveday = 7;
							}
						trangthai = counter;
					}
			  if (trangthai = counter)
					{
					  if (saveday > 7)
						{
						  saveday = 7;
						}
					}
		 	lcd_clear(&p_lcd);
	  		if (saveday == 1)
			{
			lcd_set_cursor(&p_lcd, 0, 0);
			sprintf(time, "CN%d-%d-20%d", savedate, savemonth, saveyear);
			lcd_send_string(&p_lcd, time);
			}
	  		else
			{
			lcd_set_cursor(&p_lcd, 0, 0);
			sprintf(time, "Thu+%d %d-%d-20%d", saveday, savedate, savemonth, saveyear);
			lcd_send_string(&p_lcd, time);
			}
	  		HAL_Delay(150);
	  		break;

	  	 case SET_YEAR:
	  		 if (trangthai < counter)
					{
					  trangthai = counter;
					  saveyear++;
							if (saveyear > 30)
								{
								saveyear = 24;
								}
						trangthai = counter;
					}
			  else if (trangthai > counter)
					{
						trangthai = counter;
						saveyear--;
						if (saveyear < 24)
							{
							saveyear = 30;
							}
						trangthai = counter;
					}
			  if (trangthai = counter)
					{
					  if (saveyear > 30)
						{
						  saveyear = 30;
						}
					}
			lcd_clear(&p_lcd);
	  		if (saveday == 1)
			{
			lcd_set_cursor(&p_lcd, 0, 0);
			sprintf(time, "CN%d-%d-20+%d" , savedate, savemonth, saveyear);
			lcd_send_string(&p_lcd, time);
			}
			else
			{
			lcd_set_cursor(&p_lcd, 0, 0);
			sprintf(time, "Thu%d %d-%d-20+%d", saveday, savedate, savemonth, saveyear);
			lcd_send_string(&p_lcd, time);
			}

	  		HAL_Delay(150);
	  		break;

	  	 case SET_MONTH:
	  		if (trangthai < counter)
					{
					  trangthai = counter;
					  savemonth++;
							if ( savemonth > 12)
								{
								 savemonth = 1;
								}
						trangthai = counter;
					}
			  else if (trangthai > counter)
					{
						trangthai = counter;
						 savemonth--;
						if ( savemonth < 1)
							{
							 savemonth = 12;
							}
						trangthai = counter;
					}
			  if (trangthai = counter)
					{
					  if ( savemonth > 12)
						{
						  savemonth = 12;
						}
					}
		 	lcd_clear(&p_lcd);
			if (saveday == 1)
			{
			lcd_set_cursor(&p_lcd, 0, 0);
			sprintf(time, "CN%d-+%d-20%d" , savedate, savemonth, saveyear);
			lcd_send_string(&p_lcd, time);
			}
			else
			{
			lcd_set_cursor(&p_lcd, 0, 0);
			sprintf(time, "Thu%d %d-+%d-20%d", saveday, savedate, savemonth, saveyear);
			lcd_send_string(&p_lcd, time);
			}

			HAL_Delay(150);
			break;

	  	 case SET_DATE:
	  		if (trangthai < counter)
			{
			  trangthai = counter;
			  savedate++;
				  if (savemonth == 1
						|| savemonth == 3
						|| savemonth == 5
						|| savemonth == 7
						|| savemonth == 8
						|| savemonth == 10
						|| savemonth == 12 )
					{
					if (savedate > 31)
						{
						savedate = 1;
						}
					}
				if (savemonth == 2 && saveyear == 24)
					{
					if (savedate > 29)
						{
						savedate = 1;
						}
					}
				else if (savemonth == 2 && saveyear == 28)
					{
					if (savedate > 29)
						{
						savedate = 1;
						}
					}
				else if (savemonth == 2 && saveyear != 24)
					{
					if (savedate > 28)
						{
						savedate = 1;
						}
					}
				else if (savemonth == 2 && saveyear != 28)
					{
					if (savedate > 28)
						{
						savedate = 1;
						}
					}
				if (savemonth == 4
						|| savemonth == 6
						|| savemonth == 3
						|| savemonth == 9
						|| savemonth == 11)
					{
					if (savedate > 30)
						{
						savedate = 1;
						}
					}
				trangthai = counter;
			}
	  else if (trangthai > counter)
			{
				trangthai = counter;
				savedate--;
					  if (savemonth == 1
							|| savemonth == 3
							|| savemonth == 5
							|| savemonth == 7
							|| savemonth == 8
							|| savemonth == 10
							|| savemonth == 12 )
						{
						if (savedate < 1)
							{
							savedate = 31;
							}
						}
					if (savemonth == 2 && saveyear == 24)
						{
						if (savedate < 1)
							{
							savedate = 29;
							}
						}
					else if (savemonth == 2 && saveyear == 28)
						{
						if (savedate < 1)
							{
							savedate = 29;
							}
						}
					else if (savemonth == 2 && saveyear != 24)
						{
						if (savedate < 1)
							{
							savedate = 28;
							}
						}
					else if (savemonth == 2 && saveyear != 28)
						{
						if (savedate < 1)
							{
							savedate = 28;
							}
						}
					if (savemonth == 4
							|| savemonth == 6
							|| savemonth == 3
							|| savemonth == 9
							|| savemonth == 11)
						{
						if (savedate < 1)
							{
							savedate = 30;
							}
						}
				trangthai = counter;
			}
	  if (trangthai = counter)
			{
		  if (savemonth == 1
					|| savemonth == 3
					|| savemonth == 5
					|| savemonth == 7
					|| savemonth == 8
					|| savemonth == 10
					|| savemonth == 12 )
				{
				if (savedate > 31)
					{
					savedate = 31;
					}
				}
			if (savemonth == 2 && alarmyear == 24)
				{
				if (savedate > 29)
					{
					savedate = 29;
					}
				}
			else if (savemonth == 2 && alarmyear == 28)
				{
				if (savedate > 29)
					{
					savedate = 29;
					}
				}
			else if (savemonth == 2 && alarmyear != 24)
				{
				if (savedate > 28)
					{
					savedate = 28;
					}
				}
			else if (savemonth == 2 && alarmyear != 28)
				{
				if (savedate > 28)
					{
					savedate = 28;
					}
				}
			if (savemonth == 4
					|| savemonth == 6
					|| savemonth == 3
					|| savemonth == 9
					|| savemonth == 11)
				{
				if (savedate > 30)
					{
					savedate = 30;
					}
				}
			}
	  		lcd_clear(&p_lcd);
	  		if (saveday == 1)
			{
			lcd_set_cursor(&p_lcd, 0, 0);
			sprintf(time, "CN+%d-%d-20%d" , savedate, savemonth, saveyear);
			lcd_send_string(&p_lcd, time);
			}
			else
			{
			lcd_set_cursor(&p_lcd, 0, 0);
			sprintf(time, "Thu%d +%d-%d-20%d", saveday, savedate, savemonth, saveyear);
			lcd_send_string(&p_lcd, time);
			}

			HAL_Delay(150);
			break;
	  	default:
	  				 break;
	  }

 switch(alarm)
	{
		 case ALARM_HOUR:
			 if (trangthai < counter)
					{
					  trangthai = counter;
					  savealarmhour++;
							if (savealarmhour > 23)
								{
								savealarmhour = 0;
								}
						trangthai = counter;
					}
			  else if (trangthai > counter)
					{
						trangthai = counter;
						savealarmhour--;
						if (savealarmhour < 0)
							{
							savealarmhour = 23;
							}
						trangthai = counter;
					}
			  if (trangthai = counter)
					{
					  if (savealarmhour > 23)
						{
						  savealarmhour = 23;
						}
					}
			lcd_clear(&p_lcd);
			lcd_set_cursor(&p_lcd, 2, 0);
			sprintf(time, " +%02d:%02d",  savealarmhour, savealarmmin);
			lcd_send_string(&p_lcd, time);
			HAL_Delay(300);
			break;


		 case ALARM_MIN:
			 if (trangthai < counter)
					{
					  trangthai = counter;
					  savealarmmin++;
							if (savealarmmin > 59)
								{
								savealarmmin = 0;
								}
						trangthai = counter;
					}
			  else if (trangthai > counter)
					{
						trangthai = counter;
						savealarmmin--;
						if (savealarmmin < 0)
							{
							savealarmmin = 59;
							}
						trangthai = counter;
					}
			  if (trangthai = counter)
					{
					  if (savealarmmin > 59)
						{
						  savealarmmin = 59;
						}
					}
			lcd_clear(&p_lcd);
			lcd_set_cursor(&p_lcd, 2, 0);
			sprintf(time, "  %02d:+%02d", savealarmhour, savealarmmin);
			lcd_send_string(&p_lcd, time);
		    HAL_Delay(300);
			break;



		 case ALARM_DAY:
			 if (trangthai < counter)
					{
					  trangthai = counter;
					  alarmday++;
							if (alarmday > 7)
								{
								alarmday = 1;
								}
						trangthai = counter;
					}
			  else if (trangthai > counter)
					{
						trangthai = counter;
						alarmday--;
						if (alarmday < 1)
							{
							alarmday = 7;
							}
						trangthai = counter;
					}
			  if (trangthai = counter)
					{
					  if (alarmday > 7)
						{
						  alarmday = 7;
						}
					}
			lcd_clear(&p_lcd);
			if (alarmday == 1)
			{
			lcd_set_cursor(&p_lcd, 0, 0);
			sprintf(time, "CN %d-%d-20%d", alarmdate, alarmmonth, alarmyear);
			lcd_send_string(&p_lcd, time);
			}
			else
			{
			lcd_set_cursor(&p_lcd, 0, 0);
			sprintf(time, "Thu +%d %d-%d-20%d", alarmday, alarmdate, alarmmonth, alarmyear);
			lcd_send_string(&p_lcd, time);
			}
			HAL_Delay(300);
			break;


		 case ALARM_YEAR:
			 if (trangthai < counter)
					{
					  trangthai = counter;
					  alarmyear++;
							if (alarmyear > 30)
								{
								alarmyear = 24;
								}
						trangthai = counter;
					}
			  else if (trangthai > counter)
					{
						trangthai = counter;
						alarmyear--;
						if (alarmyear < 24)
							{
							alarmyear = 30;
							}
						trangthai = counter;
					}
			  if (trangthai = counter)
					{
					  if (alarmyear > 30)
						{
						  alarmyear = 30;
						}
					}
			lcd_clear(&p_lcd);
			if (alarmday == 1)
			{
			lcd_set_cursor(&p_lcd, 0, 0);
			sprintf(time, "CN %d-%d-20+%d" , alarmdate, alarmmonth, alarmyear);
			lcd_send_string(&p_lcd, time);
			}
			else
			{
			lcd_set_cursor(&p_lcd, 0, 0);
			sprintf(time, "Thu %d %d-%d-20+%d", alarmday, alarmdate, alarmmonth, alarmyear);
			lcd_send_string(&p_lcd, time);
			}
			HAL_Delay(300);
			break;


		 case ALARM_MONTH:
	  if (trangthai < counter)
			{
			  trangthai = counter;
			  alarmmonth++;
					if (alarmmonth > 12)
						{
						alarmmonth = 1;
						}
				trangthai = counter;
			}
	  else if (trangthai > counter)
			{
				trangthai = counter;
				alarmmonth--;
				if (alarmmonth < 1)
					{
					alarmmonth = 12;
					}
				trangthai = counter;
			}
	  if (trangthai = counter)
			{
			  if (alarmmonth > 12)
				{
				  alarmmonth = 12;
				}
			}
			lcd_clear(&p_lcd);
			if (alarmday ==1)
			{
			lcd_set_cursor(&p_lcd, 0, 0);
			sprintf(time, "CN %d-+%d-20%d" , alarmdate, alarmmonth, alarmyear);
			lcd_send_string(&p_lcd, time);
			}
			else
			{
			lcd_set_cursor(&p_lcd, 0, 0);
			sprintf(time, "Thu %d %d-+%d-20%d", alarmday, alarmdate, alarmmonth, alarmyear);
			lcd_send_string(&p_lcd, time);
			}
			HAL_Delay(300);
			break;


		 case ALARM_DATE:
			 if (trangthai < counter)
					{
					  trangthai = counter;
					  alarmdate++;
						  if (alarmmonth == 1
								|| alarmmonth == 3
								|| alarmmonth == 5
								|| alarmmonth == 7
								|| alarmmonth == 8
								|| alarmmonth == 10
								|| alarmmonth == 12 )
							{
							if (alarmdate > 31)
								{
								alarmdate = 1;
								}
							}
						if (alarmmonth == 2 && alarmyear == 24)
							{
							if (alarmdate > 29)
								{
								alarmdate = 1;
								}
							}
						else if (alarmmonth == 2 && alarmyear == 28)
							{
							if (alarmdate > 29)
								{
								alarmdate = 1;
								}
							}
						else if (alarmmonth == 2 && alarmyear != 24)
							{
							if (alarmdate > 28)
								{
								alarmdate = 1;
								}
							}
						else if (alarmmonth == 2 && alarmyear != 28)
							{
							if (alarmdate > 28)
								{
								alarmdate = 1;
								}
							}
						if (alarmmonth == 4
								|| alarmmonth == 6
								|| alarmmonth == 3
								|| alarmmonth == 9
								|| alarmmonth == 11)
							{
							if (alarmdate > 30)
								{
								alarmdate = 1;
								}
							}
						trangthai = counter;
					}
			  else if (trangthai > counter)
					{
						trangthai = counter;
						alarmdate--;
							  if (alarmmonth == 1
									|| alarmmonth == 3
									|| alarmmonth == 5
									|| alarmmonth == 7
									|| alarmmonth == 8
									|| alarmmonth == 10
									|| alarmmonth == 12 )
								{
								if (alarmdate < 1)
									{
									alarmdate = 31;
									}
								}
							if (alarmmonth == 2 && alarmyear == 24)
								{
								if (alarmdate < 1)
									{
									alarmdate = 29;
									}
								}
							else if (alarmmonth == 2 && alarmyear == 28)
								{
								if (alarmdate < 1)
									{
									alarmdate = 29;
									}
								}
							else if (alarmmonth == 2 && alarmyear != 24)
								{
								if (alarmdate < 1)
									{
									alarmdate = 28;
									}
								}
							else if (alarmmonth == 2 && alarmyear != 28)
								{
								if (alarmdate < 1)
									{
									alarmdate = 28;
									}
								}
							if (alarmmonth == 4
									|| alarmmonth == 6
									|| alarmmonth == 3
									|| alarmmonth == 9
									|| alarmmonth == 11)
								{
								if (alarmdate < 1)
									{
									alarmdate = 30;
									}
								}
						trangthai = counter;
					}
			  if (trangthai = counter)
					{
				  if (alarmmonth == 1
							|| alarmmonth == 3
							|| alarmmonth == 5
							|| alarmmonth == 7
							|| alarmmonth == 8
							|| alarmmonth == 10
							|| alarmmonth == 12 )
						{
						if (alarmdate > 31)
							{
							alarmdate = 31;
							}
						}
					if (alarmmonth == 2 && alarmyear == 24)
						{
						if (alarmdate > 29)
							{
							alarmdate = 29;
							}
						}
					else if (alarmmonth == 2 && alarmyear == 28)
						{
						if (alarmdate > 29)
							{
							alarmdate = 29;
							}
						}
					else if (alarmmonth == 2 && alarmyear != 24)
						{
						if (alarmdate > 28)
							{
							alarmdate = 28;
							}
						}
					else if (alarmmonth == 2 && alarmyear != 28)
						{
						if (alarmdate > 28)
							{
							alarmdate = 28;
							}
						}
					if (alarmmonth == 4
							|| alarmmonth == 6
							|| alarmmonth == 3
							|| alarmmonth == 9
							|| alarmmonth == 11)
						{
						if (alarmdate > 30)
							{
							alarmdate = 30;
							}
						}
					}
			lcd_clear(&p_lcd);
			if (alarmday == 1)
			{
			lcd_set_cursor(&p_lcd, 0, 0);
			sprintf(time, "CN +%d-%d-20%d" , alarmdate, alarmmonth, alarmyear);
			lcd_send_string(&p_lcd, time);
			}
			else
			{
			lcd_set_cursor(&p_lcd, 0, 0);
			sprintf(time, "Thu %d +%d-%d-20%d", alarmday, alarmdate, alarmmonth, alarmyear);
			lcd_send_string(&p_lcd, time);
			}
			HAL_Delay(300);
			break;

		 default:
			 break;
	  	 }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 71;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA9 PA11 PA12
                           PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
