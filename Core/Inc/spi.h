/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#define USE_SPI1

#define ILI9341_SPI           				 &hspi2
#define ILI9341_TOUCH_SPI_PORT 				 ILI9341_SPI

typedef enum {
	TM_SPI_DataSize_8b, /*!< SPI in 8-bits mode */
	TM_SPI_DataSize_16b /*!< SPI in 16-bits mode */
} TM_SPI_DataSize_t;
/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi2;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_SPI2_Init(void);

/* USER CODE BEGIN Prototypes */
TM_SPI_DataSize_t TM_SPI_SetDataSize(SPI_HandleTypeDef* SPIx, TM_SPI_DataSize_t DataSize);
void TM_SPI_Send16bitsData(SPI_HandleTypeDef* SPIx, uint16_t value, uint16_t count);
void TM_SPI_Send8bitsData(SPI_HandleTypeDef* SPIx, uint8_t data);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

