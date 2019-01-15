/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>

#include "lcd5110.h"
#include "rtc.h"
#include "keyb.h"
#include "queue.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t aTxBuffer[18];
uint8_t tim = 0, menu = 0, input = 0, rep = 0, ch = 0, cm = 0, cs = 0, cd = 0, cdt =0, cmn = 0, cy = 0, hash = 4, hint = 0;
int16_t ax, bx, ay, by, az, bz, ae, be;
keyboard board;
LCD5110_display lcd1, lcd2;

Queue xq = {{}, 0};
Queue *q = &xq;

const char *days[7] = {"    Monday", "   Tuesday", "  Wednesday", "  Thursday", "    Friday", "  Saturday", "    Sunday"};
const char *mon[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//check what button was pressed
	keyboard_on_input(&board);
}

//is called after number is pressed
static void on_number(int x)
{
	if (menu == 1) {
		if (input == 0) {
			ch = (ch * 10 + x) % 100;
		} else if (input == 1) {
			cm = (cm * 10 + x) % 100;
		} else if (input == 2) {
			cd = x;
		} else if (input == 3) {
			rep = x;
		}
	} else if (menu == 2) {
		if (input == 0) {
			cm = (cm * 10 + x) % 100;
		} else if (input == 1) {
			cs = (cs * 10 + x) % 100;
		}
	} else if (menu == 3) {
		if (input == 0) {
			ch = (ch * 10 + x) % 100;
		} else if (input == 1) {
			cm = (cm * 10 + x) % 100;
		} else if (input == 2) {
			cs = (cs * 10 + x) % 100;
		} else if (input == 3) {
			cd = x;
		} else if (input == 4) {
			cdt = (cdt * 10 + x) % 100;
		} else if (input == 5) {
			cmn = (cmn * 10 + x) % 100;
		} else if (input == 6) {
			cy = (cy * 10 + x) % 100;
		}
	} else if (menu == 4) {
		if (input == 0) {
			if (ae < 0) az = -1 * ((abs(az) * 10 + x) % 1000);
			else az = (az * 10 + x) % 1000;
		} else if (input == 1) {
			if (be < 0) bz = -1 * ((abs(bz) * 10 + x) % 1000);
			else bz = (bz * 10 + x) % 1000;
		}
	}
}

//is called after non-number button is pressed
static void on_choice(button x)
{
	if (menu == 0) {
		if (x == button_a) {
			menu = 1; input = 0;
			ch = 0; cm = 0; cd = 0; rep = 0;
		} else if (x == button_b) {
			menu = 2; input = 0;
			cm = 0; cs = 0;
		} else if (x == button_c) {
			menu = 3; input = 0;
			ch = 0; cm = 0; cs = 0; cd = 0; cdt = 0; cmn = 0; cy = 0;
		} else if (x == button_hash) {
			reset_alarms();
			clear_queue(q);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 0);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);
			tim = 0;
		} else if ((tim == 2) && (x == button_star)) {
			tim = 0;
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 0);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);
		}
	} else if (menu == 1) {
		if (x == button_a)
			input = (input + 1) % 4;
		else if (x == button_b) {
			if (input == 0) {
				input = 3;
			} else input--;
		} else if (x == button_c) {
			menu = 0; input = 0; ch = 0; cm = 0; cd = 0; rep = 0;
		} else if (x == button_d) {
			input = 0; menu = 0;
			if (!(insert(q, cm, ch, cd, rep)))
				set_alarm(q->queue[0].min, q->queue[0].hour, q->queue[0].dotw);
			ch = 0; cm = 0; cd = 0; rep = 0;
		}
	} else if (menu == 2) {
		if ((x == button_a) || (x == button_b))
			input = (input + 1) % 2;
		else if (x == button_c) {
			menu = 0; input = 0; cs = 0; cm = 0;
		} else if (x == button_d) {
			input = 0; tim = 1; menu = 0;
			set_timer(cs, cm);
			cs = 0; cm = 0;
		}
	} else if (menu == 3) {
		if (x == button_a)
			input = (input + 1) % 7;
		else if (x == button_b) {
			if (input == 0) {
				input = 6;
			} else
				input--;
		} else if (x == button_c) {
			menu = 0; input = 0;
			ch = 0; cm = 0; cs = 0; cd = 0; cdt = 0; cmn = 0; cy = 0;
		} else if (x == button_d) {
			set_time(cs, cm, ch, cd, cdt, cmn, cy);
			menu = 0; input = 0;
			ch = 0; cm = 0; cs = 0; cd = 0; cdt = 0; cmn = 0; cy = 0;
		}
	} else if (menu == 4) {
		if ((x == button_a) || (x == button_b))
			input = (input + 1) % 2;
		else if (x == button_d) {
			if ((az == ae) && (bz == be)) {
				menu = 0; input = 0;
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);
			} else {
				input = 0;
				ax = (rand() % 40) - 20; bx = (rand() % 40) - 20;
				ay = (rand() % 40) - 20; by = (rand() % 40) - 20;
				ae = ax*ay - bx*by; be = ax*by + bx*ay;
				az = 0; bz = 0;
				hint = 0; hash = 4;
			}
		}
		else if (x == button_hash) {
			if (hash)
				hash--;
			else {
				hash = 4;
				hint = 1;
			}
		}
	}
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t sec, min, hour, day, date, month, year, tim_sec, tim_min;
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
  MX_SPI2_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
  	//first lcd5110 display init
	lcd1.hw_conf.spi_handle = &hspi2;
	lcd1.hw_conf.spi_cs_pin =  LCD1_CS_Pin;
	lcd1.hw_conf.spi_cs_port = LCD1_CS_GPIO_Port;
	lcd1.hw_conf.rst_pin =  LCD1_RST_Pin;
	lcd1.hw_conf.rst_port = LCD1_RST_GPIO_Port;
	lcd1.hw_conf.dc_pin =  LCD1_DC_Pin;
	lcd1.hw_conf.dc_port = LCD1_DC_GPIO_Port;
	lcd1.def_scr = lcd5110_def_scr;
	LCD5110_init(&lcd1.hw_conf, LCD5110_NORMAL_MODE, 0x40, 2, 3);

	//second lcd5110 display init
	lcd2.hw_conf.spi_handle = &hspi3;
	lcd2.hw_conf.spi_cs_pin =  LCD2_CS_Pin;
	lcd2.hw_conf.spi_cs_port = LCD2_CS_GPIO_Port;
	lcd2.hw_conf.rst_pin =  LCD2_RST_Pin;
	lcd2.hw_conf.rst_port = LCD2_RST_GPIO_Port;
	lcd2.hw_conf.dc_pin =  LCD2_DC_Pin;
	lcd2.hw_conf.dc_port = LCD2_DC_GPIO_Port;
	lcd2.def_scr = lcd5110_def_scr;
	LCD5110_init(&lcd2.hw_conf, LCD5110_NORMAL_MODE, 0x40, 2, 3);

	//4x4 matrix keyboard init
	uint16_t output_pins[] = {GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11};
	uint16_t input_pins[] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3};
	keyboard new_board = {GPIOD, GPIOD, input_pins, output_pins, *on_number, *on_choice};
	board = new_board;
	keyboard_init(&board);

	//reset system
	reset_alarms();
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 0);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	get_buffer();

	sec = toDEC(aTxBuffer[0]);
	min = toDEC(aTxBuffer[1]);
	hour = toDEC(aTxBuffer[2]);
	day = toDEC(aTxBuffer[3]);
	date = toDEC(aTxBuffer[4]);
	month = toDEC(aTxBuffer[5]);
	year = toDEC(aTxBuffer[6]);
	tim_sec = toDEC(aTxBuffer[7]);
	tim_min = toDEC(aTxBuffer[8]);

	//timer activation
	if (tim && (aTxBuffer[15] & 0b1)) {
		reset_flag1();
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 1);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 1);
		tim = 2;
	}

	//alarm activation
	if (q->size && (aTxBuffer[15] & 0b10)) {
		reset_flag2();
		Alarm al = next(q);
		set_alarm(al.min, al.hour, al.dotw);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 1);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 1);
		ax = (rand() % 41) - 20; bx = (rand() % 41) - 20;
		ay = (rand() % 41) - 20; by = (rand() % 41) - 20;
		az = 0; bz = 0;
		ae = ax*ay - bx*by; be = ax*by + bx*ay;
		menu = 4; input = 0; hint = 0;
	}

	//first display info
	LCD5110_clear(&lcd1);
	LCD5110_printf(&lcd1, BLACK, "   %02d:%02d:%02d\n %s\n % 2d %s 20%02d\n", hour, min, sec, days[day-1 % 7], date, mon[month-1 % 12], year);
	if (tim == 0)
		LCD5110_print("TIMER   ", BLACK, &lcd1);
	else if (tim == 1) {
		uint8_t sec_left = tim_sec - sec, min_left = tim_min - min;
		if (sec_left > 59) {
			sec_left += 60;
			min_left--;
		}
		if (min_left > 59)
			min_left += 60;
		LCD5110_printf(&lcd1, BLACK, "%02d:%02d   ", min_left, sec_left);
	} else if (tim == 2) {
		LCD5110_print("TIMER", WHITE, &lcd1);
		LCD5110_print("   ", BLACK, &lcd1);
	}
	if (q->size)
		LCD5110_print("ALARM\n", WHITE, &lcd1);
	else
		LCD5110_print("ALARM\n", BLACK, &lcd1);

	//second display info
	LCD5110_clear(&lcd2);
	if (menu == 0)
		LCD5110_print("A -> ALARM\nB -> TIMER\nC -> TIME\n# -> RESET\n", BLACK, &lcd2);
	else if (menu == 1)
		LCD5110_printf(&lcd2, BLACK, "SET ALARM:\nHH:MM D R\n%02d:%02d %d %d\n", ch, cm, cd, rep);
	else if (menu == 2)
		LCD5110_printf(&lcd2, BLACK, "SET TIMER:\nMM:SS\n%02d:%02d\n", cm, cs);
	else if (menu == 3)
		LCD5110_printf(&lcd2, BLACK, "SET TIME:\n   %02d:%02d:%02d\n      %0d      \n % 2d  %d  20%02d\n", ch, cm, cs, cd, cdt, cmn, cy);
	else if (menu == 4) {
		if (hint) LCD5110_printf(&lcd2, BLACK, "%d + %di\n%d + %di\n%d + %di\n%d + %di\n", ax, bx, ay, by, ae, be, az, bz);
		else LCD5110_printf(&lcd2, BLACK, "%d + %di\n%d + %di\n-------------\n%d + %di\n", ax, bx, ay, by, az, bz);
	}

	HAL_Delay(100);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /**Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
