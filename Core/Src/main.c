/* USER CODE BEGIN Header */
/*
 LCD display test program

 Copyright (c) 2017 Larry Bank
 email: bitbank@pobox.com
 Project started 5/17/2017

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "memorymap.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "tm_stm32f4_ili9341.h"
#include "my_image_samples.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
te_screen current_screen = SND_LOGO;
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

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  TM_ILI9341_Init();

  TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_1);

  current_screen = MAIN;

  //FIll lcd with color
  TM_ILI9341_Fill(num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));

  //just a show with the logo increasing in size
 /* while(1)
  {
	  for(int cnt=30;cnt<=100;cnt+=10){
		  draw_SND_logo(cnt,160,120,ILI9341_COLOR_GREEN);
		  HAL_Delay(1000);
		  draw_SND_logo(cnt,160,120,ILI9341_COLOR_BLUE2);
		  HAL_Delay(100);
	  }
	  draw_SND_logo(100,160,120,ILI9341_COLOR_BLUE2);
  }*/

  //draw SND logos
  draw_SND_logo(20,70,70,ILI9341_COLOR_GREEN);
  draw_SND_logo(20,250,70,ILI9341_COLOR_GREEN);
  draw_SND_logo(20,70,170,ILI9341_COLOR_GREEN);
  draw_SND_logo(20,250,170,ILI9341_COLOR_GREEN);

  draw_SND_logo(40,160,120,ILI9341_COLOR_GREEN);

  HAL_Delay(1000);

  draw_SND_logo(20,70,70,ILI9341_COLOR_BLUE2);
  draw_SND_logo(20,250,70,ILI9341_COLOR_BLUE2);
  draw_SND_logo(20,70,170,ILI9341_COLOR_BLUE2);
  draw_SND_logo(20,250,170,ILI9341_COLOR_BLUE2);

  draw_SND_logo(40,160,120,ILI9341_COLOR_BLUE2);

  TM_ILI9341_Puts(125, 95, "Hello", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_GREEN), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
  TM_ILI9341_Puts(130, 118, "Geek", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_GREEN), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));

  ILI9341_draw_filled_triangle(11, 119,47, 100,47, 137,ILI9341_COLOR_GREEN);
  ILI9341_draw_filled_triangle(272, 100,272, 137,308, 119,ILI9341_COLOR_GREEN);

  int init_tick = HAL_GetTick();

  te_arrow_visib arr_visib = SHOWN;

  //draw an image
//  HAL_Delay(1000);
//  ILI9341_DrawImage((ILI9341_WIDTH - 240) / 2, (ILI9341_HEIGHT - 240) / 2, 240, 240,  (const uint16_t*)test_img_240x240);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  uint16_t x, y;

	  if(abs(init_tick - HAL_GetTick()) >=500)
	  {
		  if( arr_visib == SHOWN)
		  {
			  ILI9341_draw_filled_triangle(11, 119,47, 100,47, 137,ILI9341_COLOR_BLUE2);
			  ILI9341_draw_filled_triangle(272, 100,272, 137,308, 119,ILI9341_COLOR_BLUE2);
			  arr_visib = HIDDEN;
		  }
		  else
		  {
			  ILI9341_draw_filled_triangle(11, 119,47, 100,47, 137,ILI9341_COLOR_GREEN);
			  ILI9341_draw_filled_triangle(272, 100,272, 137,308, 119,ILI9341_COLOR_GREEN);
			  arr_visib = SHOWN;
		  }

		  init_tick = HAL_GetTick();
	  }

	  if(ILI9341_TouchGetCoordinates(&x, &y)) {
	  	  	  if(x>=0 && x<=55 && y<=130 && y>=90)
			  {
	  			  if(current_screen == MAIN) // right arrow
	  			  {
	  				  current_screen = MEASUREMENTS;

	  				  TM_ILI9341_Puts(125, 95, "Hello", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
	  				  TM_ILI9341_Puts(130, 118, "Geek", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));

	  				  TM_ILI9341_Puts(20, 5, "Procs Parameters", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_GREEN), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
	  				  TM_ILI9341_Puts(80, 50, "F objc: 500kN\nR objc: 20 cm\nC objc: 24 mF", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_WHITE), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));

	  			  }
	  			  else if(current_screen == MEASUREMENTS )
	  			  {
	  				  current_screen = CYCLE_INFO;
	  				  TM_ILI9341_Puts(20, 5, "Procs Parameters", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
	  				  TM_ILI9341_Puts(80, 50, "F objc: 500kN\nR objc: 20 cm\nC objc: 24 mF", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));

	  				  TM_ILI9341_Puts(20, 5, " Cycle Parameters", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_GREEN), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
	  				  TM_ILI9341_Puts(80, 40,"Cycle state", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_GREEN), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
	  				  TM_ILI9341_DrawRectangle(80, 62, 200, 126, num_swap_16b(ILI9341_COLOR_GREEN));
	  				  TM_ILI9341_Puts(83, 67, "0-1s init\n10  objc\nSilver", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_WHITE), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));

					  TM_ILI9341_Puts(83, 130, "Cycle Time: ", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_GREEN), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
	  				  TM_ILI9341_DrawRectangle(83, 152, 230, 168, num_swap_16b(ILI9341_COLOR_GREEN));
	  				  TM_ILI9341_DrawFilledRectangle(83, 152, 100, 168, num_swap_16b(ILI9341_COLOR_GREEN));

	  				  TM_ILI9341_Puts(83, 170, "Procs Time: ", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_GREEN), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
	  				  TM_ILI9341_DrawRectangle(83, 190, 230, 206, num_swap_16b(ILI9341_COLOR_GREEN));
	  				  TM_ILI9341_DrawFilledRectangle(83, 190, 200, 206, num_swap_16b(ILI9341_COLOR_GREEN));

	  			  }
	  			  else if(current_screen == CYCLE_INFO )
	  			  {
	  				 current_screen = MAIN;
	  				 TM_ILI9341_Puts(20, 5, " Cycle Parameters", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
					 TM_ILI9341_Puts(80, 40,"Cycle state", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
					 TM_ILI9341_DrawRectangle(80, 62, 200, 126, num_swap_16b(ILI9341_COLOR_BLUE2));
	  				  TM_ILI9341_Puts(83, 67, "0-1s init\n10  objc\nSilver", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));

					  TM_ILI9341_Puts(83, 130, "Cycle Time: ", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
	  				  TM_ILI9341_DrawRectangle(83, 152, 230, 168, num_swap_16b(ILI9341_COLOR_BLUE2));
	  				  TM_ILI9341_DrawFilledRectangle(83, 152, 100, 168, num_swap_16b(ILI9341_COLOR_BLUE2));

	  				  TM_ILI9341_Puts(83, 170, "Procs Time: ", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
	  				  TM_ILI9341_DrawRectangle(83, 190, 230, 206, num_swap_16b(ILI9341_COLOR_BLUE2));
	  				  TM_ILI9341_DrawFilledRectangle(83, 190, 200, 206, num_swap_16b(ILI9341_COLOR_BLUE2));

	  				  TM_ILI9341_Puts(125, 95, "Hello", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_GREEN), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
	  				  TM_ILI9341_Puts(130, 118, "Geek", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_GREEN), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));

	  			  }

	  		  }
	  	  	  else if(x>=210 && x<=319 && y<=137 && y>=100 ) //left arrow
	  		  {
	  			  if(current_screen == MAIN)
	  			  {
	  				 current_screen = CYCLE_INFO;
	  				 TM_ILI9341_Puts(125, 95, "Hello", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
	  				 TM_ILI9341_Puts(130, 118, "Geek", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));

					 TM_ILI9341_Puts(20, 5, " Cycle Parameters", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_GREEN), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
					 TM_ILI9341_Puts(80, 40,"Cycle state", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_GREEN), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
					 TM_ILI9341_DrawRectangle(80, 62, 200, 126, num_swap_16b(ILI9341_COLOR_GREEN));
					 TM_ILI9341_Puts(83, 67, "0-1s init\n10  objc\nSilver", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_WHITE), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));

					 TM_ILI9341_Puts(83, 130, "Cycle Time: ", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_GREEN), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
					  TM_ILI9341_DrawRectangle(83, 152, 230, 168, num_swap_16b(ILI9341_COLOR_GREEN));
					  TM_ILI9341_DrawFilledRectangle(83, 152, 100, 168, num_swap_16b(ILI9341_COLOR_GREEN));

					  TM_ILI9341_Puts(83, 170, "Procs Time: ", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_GREEN), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
					  TM_ILI9341_DrawRectangle(83, 190, 230, 206, num_swap_16b(ILI9341_COLOR_GREEN));
					  TM_ILI9341_DrawFilledRectangle(83, 190, 200, 206, num_swap_16b(ILI9341_COLOR_GREEN));

	  			  }
	  			  else if(current_screen == CYCLE_INFO)
	  			  {
	  				current_screen = MEASUREMENTS;

	  				 TM_ILI9341_Puts(20, 5, " Cycle Parameters", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
					 TM_ILI9341_Puts(80, 40,"Cycle state", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
					 TM_ILI9341_DrawRectangle(80, 62, 200, 126, num_swap_16b(ILI9341_COLOR_BLUE2));
					 TM_ILI9341_Puts(83, 67, "0-1s init\n10  objc\nSilver", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));

					 TM_ILI9341_Puts(83, 130, "Cycle Time: ", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
					  TM_ILI9341_DrawRectangle(83, 152, 230, 168, num_swap_16b(ILI9341_COLOR_BLUE2));
					  TM_ILI9341_DrawFilledRectangle(83, 152, 100, 168, num_swap_16b(ILI9341_COLOR_BLUE2));

					  TM_ILI9341_Puts(83, 170, "Cycle Time: ", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
					  TM_ILI9341_DrawRectangle(83, 190, 230, 206, num_swap_16b(ILI9341_COLOR_BLUE2));
					  TM_ILI9341_DrawFilledRectangle(83, 190, 200, 206, num_swap_16b(ILI9341_COLOR_BLUE2));

					 TM_ILI9341_Puts(20, 5, "Procs Parameters", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_GREEN), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
	  				  TM_ILI9341_Puts(80, 50, "F objc: 500kN\nR objc: 20 cm\nC objc: 24 mF", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_WHITE), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));



	  			  }
	  			  else if(current_screen == MEASUREMENTS)
	  			  {
	  				  current_screen = MAIN;
	  				  TM_ILI9341_Puts(20, 5, "Procs Parameters", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
	  				  TM_ILI9341_Puts(80, 50, "F objc: 500kN\nR objc: 20 cm\nC objc: 24 mF", &TM_FontCalibri_11x18, num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));

	  				  TM_ILI9341_Puts(125, 95, "Hello", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_GREEN), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));
					  TM_ILI9341_Puts(130, 118, "Geek", &TM_FontCalibri_16x26, num_swap_16b((uint16_t)ILI9341_COLOR_GREEN), num_swap_16b((uint16_t)ILI9341_COLOR_BLUE2));

	  			  }

	  		  }
	  		  }


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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 32;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK4|RCC_CLOCKTYPE_HCLK2
                              |RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK2Divider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.AHBCLK4Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SMPS;
  PeriphClkInitStruct.SmpsClockSelection = RCC_SMPSCLKSOURCE_HSI;
  PeriphClkInitStruct.SmpsDivSelection = RCC_SMPSCLKDIV_RANGE0;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN Smps */

  /* USER CODE END Smps */
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
