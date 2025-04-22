/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */


#include "tm_stm32f4_ili9341.h"
#include "main.h"
#include "tm_stm32f4_fonts.h"
#include "stdbool.h"
#include <stdlib.h>
#include <math.h>
/**
 * @brief  Orientation
 * @note   Used private
 */
typedef enum {
	TM_ILI9341_Landscape,
	TM_ILI9341_Portrait
} TM_ILI9341_Orientation;

/**
 * @brief  LCD options
 * @note   Used private
 */
typedef struct {
	uint16_t width;
	uint16_t height;
	TM_ILI9341_Orientation orientation; // 1 = portrait; 0 = landscape
} TM_ILI931_Options_t;

/* Pin definitions */
#define ILI9341_CS_SET				HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_SET)
#define ILI9341_CS_RESET			HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_RESET)
#define ILI9341_DC_SET				HAL_GPIO_WritePin(DC_GPIO_Port,DC_Pin,GPIO_PIN_SET)
#define ILI9341_DC_RESET			HAL_GPIO_WritePin(DC_GPIO_Port,DC_Pin,GPIO_PIN_RESET)
#define ILI9341_RST_RESET			HAL_GPIO_WritePin(RESET_GPIO_Port,RESET_Pin,GPIO_PIN_RESET)
#define ILI9341_RST_SET				HAL_GPIO_WritePin(RESET_GPIO_Port,RESET_Pin,GPIO_PIN_SET)
#define ILI9341_CS_TOUCH_RESET		HAL_GPIO_WritePin(CS_touch_GPIO_Port,CS_touch_Pin,GPIO_PIN_RESET)
#define ILI9341_CS_TOUCH_SET		HAL_GPIO_WritePin(CS_touch_GPIO_Port,CS_touch_Pin,GPIO_PIN_SET)
#define ILI9341_TouchPressed()		(HAL_GPIO_ReadPin(touch_irq_GPIO_Port, touch_irq_Pin) == GPIO_PIN_RESET)
#define ILI9341_TOUCH_MIN_RAW_X		1500
#define ILI9341_TOUCH_MIN_RAW_Y		3276
#define ILI9341_TOUCH_SCALE_X		320
#define ILI9341_TOUCH_SCALE_Y 		240
#define ILI9341_TOUCH_MAX_RAW_X     31000
#define ILI9341_TOUCH_MAX_RAW_Y		30110

/* Private defines */
#define ILI9341_RESET				0x01
#define ILI9341_SLEEP_OUT			0x11
#define ILI9341_GAMMA				0x26
#define ILI9341_DISPLAY_OFF			0x28
#define ILI9341_DISPLAY_ON			0x29
#define ILI9341_COLUMN_ADDR			0x2A
#define ILI9341_PAGE_ADDR			0x2B
#define ILI9341_GRAM				0x2C
#define ILI9341_MAC					0x36
#define ILI9341_PIXEL_FORMAT		0x3A
#define ILI9341_WDB					0x51
#define ILI9341_WCD					0x53
#define ILI9341_RGB_INTERFACE		0xB0
#define ILI9341_FRC					0xB1
#define ILI9341_BPC					0xB5
#define ILI9341_DFC					0xB6
#define ILI9341_POWER1				0xC0
#define ILI9341_POWER2				0xC1
#define ILI9341_VCOM1				0xC5
#define ILI9341_VCOM2				0xC7
#define ILI9341_POWERA				0xCB
#define ILI9341_POWERB				0xCF
#define ILI9341_PGAMMA				0xE0
#define ILI9341_NGAMMA				0xE1
#define ILI9341_DTCA				0xE8
#define ILI9341_DTCB				0xEA
#define ILI9341_POWER_SEQ			0xED
#define ILI9341_3GAMMA_EN			0xF2
#define ILI9341_INTERFACE			0xF6
#define ILI9341_PRC					0xF7
#define ILI9341_INVERS_ON			0x21

/* Pin functions */
uint16_t ILI9341_x;
uint16_t ILI9341_y;
TM_ILI931_Options_t ILI9341_Opts;
uint8_t ILI9341_INT_CalledFromPuts = 0;
_Bool volatile is_there_touch = false;

/* Private functions */
void TM_ILI9341_InitLCD(void);
void TM_ILI9341_SendData(uint8_t data);
void TM_ILI9341_SendCommand(uint8_t data);
void TM_ILI9341_Delay(volatile unsigned int delay);
void TM_ILI9341_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void TM_ILI9341_INT_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

void TM_ILI9341_Init() {
	/* CS high */
	ILI9341_CS_SET;
	
	/* DC high */
	ILI9341_DC_SET;

	/*unselect touch */
	ILI9341_CS_TOUCH_SET;

	/* Force reset */
	ILI9341_RST_SET;

	TM_ILI9341_Delay(5);

	ILI9341_RST_RESET;
	/* Delay for RST response */
	TM_ILI9341_Delay(20);

	ILI9341_RST_SET;
	TM_ILI9341_Delay(150);

	/* Init LCD */
	TM_ILI9341_InitLCD();	
	
	/* Set default settings */
	ILI9341_x = ILI9341_y = 0;
	ILI9341_Opts.width = ILI9341_WIDTH;
	ILI9341_Opts.height = ILI9341_HEIGHT;
	ILI9341_Opts.orientation = TM_ILI9341_Landscape;
	
	HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);

	/* Fill with white color */
	//TM_ILI9341_Fill(ILI9341_COLOR_WHITE);
}

void TM_ILI9341_InitLCD(void) {
	/* Software reset */
	TM_ILI9341_SendCommand(ILI9341_RESET);
	TM_ILI9341_Delay(120);
	
	TM_ILI9341_SendCommand(ILI9341_POWERA);
	TM_ILI9341_SendData(0x39);
	TM_ILI9341_SendData(0x2C);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x34);
	TM_ILI9341_SendData(0x02);
	TM_ILI9341_SendCommand(ILI9341_POWERB);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0xC1);
	TM_ILI9341_SendData(0x30);
	TM_ILI9341_SendCommand(ILI9341_DTCA);
	TM_ILI9341_SendData(0x85);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x78);
	TM_ILI9341_SendCommand(ILI9341_DTCB);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendCommand(ILI9341_POWER_SEQ);
	TM_ILI9341_SendData(0x64);
	TM_ILI9341_SendData(0x03);
	TM_ILI9341_SendData(0x12);
	TM_ILI9341_SendData(0x81);
	TM_ILI9341_SendCommand(ILI9341_PRC);
	TM_ILI9341_SendData(0x20);
	TM_ILI9341_SendCommand(ILI9341_POWER1);
	TM_ILI9341_SendData(0x23);
	TM_ILI9341_SendCommand(ILI9341_POWER2);
	TM_ILI9341_SendData(0x10);
	TM_ILI9341_SendCommand(ILI9341_VCOM1);
	TM_ILI9341_SendData(0x3E);
	TM_ILI9341_SendData(0x28);
	TM_ILI9341_SendCommand(ILI9341_VCOM2);
	TM_ILI9341_SendData(0x86);
	TM_ILI9341_SendCommand(ILI9341_MAC);
	TM_ILI9341_SendData(0x48);
	TM_ILI9341_SendCommand(ILI9341_PIXEL_FORMAT);
	TM_ILI9341_SendData(0x55);
	TM_ILI9341_SendCommand(ILI9341_FRC);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x13);
	TM_ILI9341_SendCommand(ILI9341_DFC);
	TM_ILI9341_SendData(0x08);
	TM_ILI9341_SendData(0x82);
	TM_ILI9341_SendData(0x27);
	TM_ILI9341_SendCommand(ILI9341_3GAMMA_EN);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendCommand(ILI9341_COLUMN_ADDR);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0xEF);
	TM_ILI9341_SendCommand(ILI9341_PAGE_ADDR);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x01);
	TM_ILI9341_SendData(0x3F);
	TM_ILI9341_SendCommand(ILI9341_GAMMA);
	TM_ILI9341_SendData(0x01);
	TM_ILI9341_SendCommand(ILI9341_PGAMMA);
	TM_ILI9341_SendData(0x0F);
	TM_ILI9341_SendData(0x31);
	TM_ILI9341_SendData(0x2B);
	TM_ILI9341_SendData(0x0C);
	TM_ILI9341_SendData(0x0E);
	TM_ILI9341_SendData(0x08);
	TM_ILI9341_SendData(0x4E);
	TM_ILI9341_SendData(0xF1);
	TM_ILI9341_SendData(0x37);
	TM_ILI9341_SendData(0x07);
	TM_ILI9341_SendData(0x10);
	TM_ILI9341_SendData(0x03);
	TM_ILI9341_SendData(0x0E);
	TM_ILI9341_SendData(0x09);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendCommand(ILI9341_NGAMMA);
	TM_ILI9341_SendData(0x00);
	TM_ILI9341_SendData(0x0E);
	TM_ILI9341_SendData(0x14);
	TM_ILI9341_SendData(0x03);
	TM_ILI9341_SendData(0x11);
	TM_ILI9341_SendData(0x07);
	TM_ILI9341_SendData(0x31);
	TM_ILI9341_SendData(0xC1);
	TM_ILI9341_SendData(0x48);
	TM_ILI9341_SendData(0x08);
	TM_ILI9341_SendData(0x0F);
	TM_ILI9341_SendData(0x0C);
	TM_ILI9341_SendData(0x31);
	TM_ILI9341_SendData(0x36);
	TM_ILI9341_SendData(0x0F);
//	TM_ILI9341_SendCommand(ILI9341_INVERS_ON);

	TM_ILI9341_SendCommand(ILI9341_SLEEP_OUT);

	TM_ILI9341_Delay(120);

	TM_ILI9341_SendCommand(ILI9341_DISPLAY_ON);
	TM_ILI9341_SendCommand(ILI9341_GRAM);

}

void TM_ILI9341_DisplayOn(void) {
	TM_ILI9341_SendCommand(ILI9341_DISPLAY_ON);
}

void TM_ILI9341_DisplayOff(void) {
	TM_ILI9341_SendCommand(ILI9341_DISPLAY_OFF);
}

void TM_ILI9341_SendCommand(uint8_t data) {
	ILI9341_DC_RESET;
	ILI9341_CS_RESET;
	TM_SPI_Send8bitsData(ILI9341_SPI, data);
	ILI9341_CS_SET;
}



void TM_ILI9341_SendData(uint8_t data) {
	ILI9341_DC_SET;
	ILI9341_CS_RESET;
	TM_SPI_Send8bitsData(ILI9341_SPI, data);
	ILI9341_CS_SET;
}

void TM_ILI9341_DrawPixel(uint16_t x, uint16_t y, uint32_t color) {
	TM_ILI9341_SetCursorPosition(x, y, x, y);

	TM_ILI9341_SendCommand(ILI9341_GRAM);
	TM_ILI9341_SendData(color >> 8);
	TM_ILI9341_SendData(color & 0xFF);
}


void TM_ILI9341_SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	TM_ILI9341_SendCommand(ILI9341_COLUMN_ADDR);
	TM_ILI9341_SendData(x1 >> 8);
	TM_ILI9341_SendData(x1 & 0xFF);
	TM_ILI9341_SendData(x2 >> 8);
	TM_ILI9341_SendData(x2 & 0xFF);

	TM_ILI9341_SendCommand(ILI9341_PAGE_ADDR);
	TM_ILI9341_SendData(y1 >> 8);
	TM_ILI9341_SendData(y1 & 0xFF);
	TM_ILI9341_SendData(y2 >> 8);
	TM_ILI9341_SendData(y2 & 0xFF);
}

void TM_ILI9341_Fill(uint32_t color) {
	/* Fill entire screen */
	TM_ILI9341_INT_Fill(0, 0, ILI9341_Opts.width - 1, ILI9341_Opts.height, color);
}

void TM_ILI9341_INT_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	uint32_t pixels_count;
	
	/* Set cursor position */
	TM_ILI9341_SetCursorPosition(x0, y0, x1, y1);

	/* Set command for GRAM data */
	TM_ILI9341_SendCommand(ILI9341_GRAM);
	
	/* Calculate pixels count */
	pixels_count = (x1 - x0 + 1) * (y1 - y0 + 1);

	/* Send everything */
	ILI9341_CS_RESET;
	ILI9341_DC_SET;
	
	/* Go to 16-bit SPI mode */
	TM_SPI_SetDataSize(ILI9341_SPI, TM_SPI_DataSize_16b);
	
	/* Send first 65535 bytes, SPI MUST BE IN 16-bit MODE */
	TM_SPI_Send16bitsData(ILI9341_SPI, color, (pixels_count > 0xFFFF) ? 0xFFFF : pixels_count);
	
	/* Check again */
	if (pixels_count > 0xFFFF) {
		/* Send remaining data */
		TM_SPI_Send16bitsData(ILI9341_SPI, color, pixels_count - 0xFFFF);
	}
	
	ILI9341_CS_SET;

	/* Go back to 8-bit SPI mode */
	TM_SPI_SetDataSize(ILI9341_SPI, TM_SPI_DataSize_8b);
}

void TM_ILI9341_Delay(volatile unsigned int delay) {
	HAL_Delay(delay);
}

void TM_ILI9341_Rotate(TM_ILI9341_Orientation_t orientation) {
	TM_ILI9341_SendCommand(ILI9341_MAC);
	if (orientation == TM_ILI9341_Orientation_Landscape_1) {
		TM_ILI9341_SendData(0x58);
	} else if (orientation == TM_ILI9341_Orientation_Landscape_2) {
		TM_ILI9341_SendData(0x88);
	} else if (orientation == TM_ILI9341_Orientation_Portrait_1) {
		TM_ILI9341_SendData(0x28);
	} else if (orientation == TM_ILI9341_Orientation_Portrait_2) {
		TM_ILI9341_SendData(0xE8);
	}
	
	if (orientation == TM_ILI9341_Orientation_Landscape_1 || orientation == TM_ILI9341_Orientation_Landscape_2) {
		ILI9341_Opts.width = ILI9341_WIDTH;
		ILI9341_Opts.height = ILI9341_HEIGHT;
		ILI9341_Opts.orientation = TM_ILI9341_Landscape;
	} else {
		ILI9341_Opts.width = ILI9341_HEIGHT;
		ILI9341_Opts.height = ILI9341_WIDTH;
		ILI9341_Opts.orientation = TM_ILI9341_Portrait;
	}
}

void TM_ILI9341_Puts(uint16_t x, uint16_t y, char *str, TM_FontDef_t *font, uint32_t foreground, uint32_t background) {
	uint16_t startX = x;
	
	/* Set X and Y coordinates */
	ILI9341_x = x;
	ILI9341_y = y;
	
	while (*str) {
		/* New line */
		if (*str == '\n') {
			ILI9341_y += font->FontHeight + 1;
			/* if after \n is also \r, than go to the left of the screen */
			if (*(str + 1) == '\r') {
				ILI9341_x = 0;
				str++;
			} else {
				ILI9341_x = startX;
			}
			str++;
			continue;
		} else if (*str == '\r') {
			str++;
			continue;
		}
		
		/* Put character to LCD */
		if(*str != 0xD0 && *str != 0xD1) //get rid of UTF-8 extra byte
			TM_ILI9341_Putc(ILI9341_x, ILI9341_y, *str, font, foreground, background);
		str++;
	}
}

void TM_ILI9341_GetStringSize(char *str, TM_FontDef_t *font, uint16_t *width, uint16_t *height) {
	uint16_t w = 0;
	*height = font->FontHeight;
	while (*str++) {
		w += font->FontWidth;
	}
	*width = w;
}

void TM_ILI9341_Putc(uint16_t x, uint16_t y, char c, TM_FontDef_t *font, uint32_t foreground, uint32_t background) {
	uint32_t i, b, j;
	/* Set coordinates */
	ILI9341_x = x;
	ILI9341_y = y;
	
	if ((ILI9341_x + font->FontWidth) > ILI9341_Opts.width) {
		/* If at the end of a line of display, go to new line and set x to 0 position */
		ILI9341_y += font->FontHeight;
		ILI9341_x = 0;
	}
	
	/* Draw rectangle for background */
	TM_ILI9341_INT_Fill(ILI9341_x, ILI9341_y, ILI9341_x + font->FontWidth, ILI9341_y + font->FontHeight, background);
	
	/* Draw font data */
		for (i = 0; i < font->FontHeight; i++) {
			if(c>= 0x20 && c<=0x7E)
				b = font->data[(c - 32) * font->FontHeight + i];
			else if(c>=144 && c<192)
				b = font->data[(c -144+95) * font->FontHeight + i];
			else if(c>=128 && c<=143)
				b = font->data[(c - 80+95) * font->FontHeight + i];
			else
				continue;
			for (j = 0; j < font->FontWidth; j++) {
				if ((b << j) & 0x8000) {
					TM_ILI9341_DrawPixel(ILI9341_x + j, (ILI9341_y + i), foreground);
				}
			}
		}
	
	/* Set new pointer */
	ILI9341_x += font->FontWidth;
}


ts_x_y_table TM_ILI9341_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
	/* Code by dewoller: https://github.com/dewoller changed by Benedito */
	
	int16_t dx, dy, sx, sy, err, e2; 	
	ts_x_y_table xy_table ;
	uint8_t col_idx = 0;

	for(int idx1=0;idx1<500;idx1++)
		xy_table.x_coords[idx1] = 800;
	for(int idx1=0;idx1<500;idx1++)
		xy_table.y_coords[idx1] = 800;

	/* Check for overflow */
	if (x0 >= ILI9341_Opts.width) {
		x0 = ILI9341_Opts.width - 1;
	}
	if (x1 >= ILI9341_Opts.width) {
		x1 = ILI9341_Opts.width - 1;
	}
	if (y0 >= ILI9341_Opts.height) {
		y0 = ILI9341_Opts.height - 1;
	}
	if (y1 >= ILI9341_Opts.height) {
		y1 = ILI9341_Opts.height - 1;
	}
	
	dx = abs(x1 - x0);
	dy = abs(y1 - y0);
	
	/* Vertical or horizontal line */
	if (dx == 0 || dy == 0) {
		TM_ILI9341_INT_Fill(x0, y0, x1, y1, color);
		return xy_table;
	}
	
	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;
	err = ((dx > dy) ? dx : -dy) / 2; 

	while (1) {
		TM_ILI9341_DrawPixel(x0, y0, color); 

		xy_table.x_coords[col_idx]   = x0;
		xy_table.y_coords[col_idx++] = y0;

		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err; 
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		} 
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		} 
	}

	return xy_table;
}

void TM_ILI9341_DrawRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
	TM_ILI9341_DrawLine(x0, y0, x1, y0, color); //Top
	TM_ILI9341_DrawLine(x0, y0, x0, y1, color);	//Left
	TM_ILI9341_DrawLine(x1, y0, x1, y1, color);	//Right
	TM_ILI9341_DrawLine(x0, y1, x1, y1, color);	//Bottom
}

void TM_ILI9341_DrawFilledRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {	
	uint16_t tmp;
	
	/* Check correction */
	if (x0 > x1) {
		tmp = x0;
		x0 = x1;
		x1 = tmp;
	}
	if (y0 > y1) {
		tmp = y0;
		y0 = y1;
		y1 = tmp;
	}
	
	/* Fill rectangle */
	TM_ILI9341_INT_Fill(x0, y0, x1, y1, color);
	
	/* CS HIGH back */
	ILI9341_CS_SET;
}

void TM_ILI9341_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    TM_ILI9341_DrawPixel(x0, y0 + r, color);
    TM_ILI9341_DrawPixel(x0, y0 - r, color);
    TM_ILI9341_DrawPixel(x0 + r, y0, color);
    TM_ILI9341_DrawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        TM_ILI9341_DrawPixel(x0 + x, y0 + y, color);
        TM_ILI9341_DrawPixel(x0 - x, y0 + y, color);
        TM_ILI9341_DrawPixel(x0 + x, y0 - y, color);
        TM_ILI9341_DrawPixel(x0 - x, y0 - y, color);

        TM_ILI9341_DrawPixel(x0 + y, y0 + x, color);
        TM_ILI9341_DrawPixel(x0 - y, y0 + x, color);
        TM_ILI9341_DrawPixel(x0 + y, y0 - x, color);
        TM_ILI9341_DrawPixel(x0 - y, y0 - x, color);
    }
}

void TM_ILI9341_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    TM_ILI9341_DrawPixel(x0, y0 + r, color);
    TM_ILI9341_DrawPixel(x0, y0 - r, color);
    TM_ILI9341_DrawPixel(x0 + r, y0, color);
    TM_ILI9341_DrawPixel(x0 - r, y0, color);
    TM_ILI9341_DrawLine(x0 - r, y0, x0 + r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        TM_ILI9341_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
        TM_ILI9341_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

        TM_ILI9341_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
        TM_ILI9341_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
    }
}

void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT))
    	return;
    if((x + w - 1) >= ILI9341_WIDTH){
    	return;
    }
    if((y + h - 1) >= ILI9341_HEIGHT){
    	return;
    }

	uint32_t pixels_count;

	/* Set cursor position */
	TM_ILI9341_SetCursorPosition(x, y, x+w-1, y+h-1);

	/* Set command for GRAM data */
	TM_ILI9341_SendCommand(ILI9341_GRAM);

	/* Calculate pixels count */
	pixels_count = w*h;

	/* Send everything */
	ILI9341_CS_RESET;
	ILI9341_DC_SET;

	/* Go to 16-bit SPI mode */
	TM_SPI_SetDataSize(ILI9341_SPI, TM_SPI_DataSize_16b);

	/* Send first 65535 bytes, SPI MUST BE IN 16-bit MODE */
	for(int cnt=0;cnt<pixels_count;cnt++)
		TM_SPI_Send16bitsData(ILI9341_SPI, num_swap_16b(*data++), 1);

	/* Check again */
	if (pixels_count > 0xFFFF) {
		/* Send remaining data */
		for(int cnt=0;cnt<pixels_count - 0xFFFF;cnt++)
			TM_SPI_Send16bitsData(ILI9341_SPI, num_swap_16b(*data++), 1);
	}

	ILI9341_CS_SET;

	/* Go back to 8-bit SPI mode */
	TM_SPI_SetDataSize(ILI9341_SPI, TM_SPI_DataSize_8b);

}

_Bool ILI9341_TouchGetCoordinates(uint16_t* x, uint16_t* y)
{
    static  uint8_t cmd_read_x = 0x90;
    static  uint8_t cmd_read_y = 0xD0;
    static  uint8_t zeroes_tx[2] = { 0x00, 0x00 };
    uint8_t y_raw[2];
    uint8_t x_raw[2];

    ILI9341_CS_TOUCH_RESET;

    uint32_t avg_x = 0;
    uint32_t avg_y = 0;
    uint8_t nsamples = 0;

    for(uint8_t i = 0; i < 16; i++) {
        if(  is_there_touch != true)
            break;

        nsamples++;

        HAL_SPI_Transmit(ILI9341_TOUCH_SPI_PORT, &cmd_read_y, 1, 100);

        HAL_SPI_TransmitReceive(ILI9341_TOUCH_SPI_PORT, zeroes_tx, y_raw,  sizeof(y_raw), 100);

        HAL_SPI_Transmit(ILI9341_TOUCH_SPI_PORT, &cmd_read_x, 1, 100);

        HAL_SPI_TransmitReceive(ILI9341_TOUCH_SPI_PORT, zeroes_tx, x_raw, sizeof(x_raw), 100);

        avg_x += (((uint16_t)x_raw[0]) << 8) | ((uint16_t)x_raw[1]);
        avg_y += (((uint16_t)y_raw[0]) << 8) | ((uint16_t)y_raw[1]);
    }

    ILI9341_CS_TOUCH_SET;

    is_there_touch = false;

    if(nsamples < 16)
        return false;

    uint32_t raw_x = (avg_x / 16);

    if(raw_x < ILI9341_TOUCH_MIN_RAW_X) {
    	raw_x = ILI9341_TOUCH_MIN_RAW_X;
    }

    if(raw_x > ILI9341_TOUCH_MAX_RAW_X) {
    	raw_x = ILI9341_TOUCH_MAX_RAW_X;
    }

    uint32_t raw_y = (avg_y / 16);

    if(raw_y < ILI9341_TOUCH_MIN_RAW_Y) {
    	raw_y = ILI9341_TOUCH_MIN_RAW_Y;
    }

    if(raw_y > ILI9341_TOUCH_MAX_RAW_Y) {
    	raw_y = ILI9341_TOUCH_MAX_RAW_Y;
    }

    *x = (raw_x - ILI9341_TOUCH_MIN_RAW_X) * ILI9341_TOUCH_SCALE_X / (ILI9341_TOUCH_MAX_RAW_X - ILI9341_TOUCH_MIN_RAW_X);
    *y = (raw_y - ILI9341_TOUCH_MIN_RAW_Y) * ILI9341_TOUCH_SCALE_Y / (ILI9341_TOUCH_MAX_RAW_Y - ILI9341_TOUCH_MIN_RAW_Y);

    return true;
}

/**
 * This function can be used  with any triangle
 */
void ILI9341_draw_unfilled_triangle(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2,uint16_t color)
{
	TM_ILI9341_DrawLine(x0, y0,  x1, y1,num_swap_16b((uint16_t)color)  );

	TM_ILI9341_DrawLine(x0,y0,x2,y2,num_swap_16b((uint16_t)color)  );

	TM_ILI9341_DrawLine(x1,y1,x2,y2,num_swap_16b((uint16_t)color)  );
}

/**
 * This function should be used only with isosceles triangles !
 */

void ILI9341_draw_filled_triangle(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2,uint16_t color)
{
	uint16_t hight = x1-x0,tmp1,tmp2,tmp3;

  ts_x_y_table first_side  = TM_ILI9341_DrawLine(x0, y0,  x1, y1,num_swap_16b((uint16_t)color)  );
  ts_x_y_table second_side = TM_ILI9341_DrawLine(x0,y0,x2,y2,num_swap_16b((uint16_t)color)  );
  ts_x_y_table third_side  = TM_ILI9341_DrawLine(x1,y1,x2,y2,num_swap_16b((uint16_t)color)  );

  for(uint16_t col_idx=0; col_idx<=500;col_idx++){
	  if(first_side.x_coords[col_idx] == 800 ){
		  tmp1 = col_idx;
		  break;
	  }
  }

  for(uint16_t col_idx=0; col_idx<=500;col_idx++){
	  if(second_side.x_coords[col_idx] == 800 ){
		  tmp2 = col_idx;
		  break;
	  }
  }

  for(uint16_t col_idx=0; col_idx<=500;col_idx++){
	  if(third_side.x_coords[col_idx] == 800 ){
		  tmp3 = col_idx;
		  break;
	  }
  }

  if(tmp1==tmp2 && x0 != x1 && y0 !=y1 && y0 != y2)
  {
	  for(uint16_t col_idx=0; col_idx<=hight;col_idx++){
		  TM_ILI9341_DrawLine(first_side.x_coords[col_idx], first_side.y_coords[col_idx], second_side.x_coords[col_idx], second_side.y_coords[col_idx],num_swap_16b((uint16_t)color)  );
	  }
  }
  else if(tmp1==tmp3 && x0 != x1 && x1 !=x2 && y1 != y2 && y0 != y2)
  {
	  hight = x1-x0;
	  for(uint16_t col_idx=0; col_idx<=hight;col_idx++){
		  TM_ILI9341_DrawLine(first_side.x_coords[col_idx], first_side.y_coords[col_idx], third_side.x_coords[col_idx], third_side.y_coords[col_idx],num_swap_16b((uint16_t)color)  );
	  }
  }
  else if(tmp2==tmp3 && x0 != x2 && y0 != y2 && x1 !=x2 && y1 != y2)
  {
	  hight = x2-x0;
	  for(uint16_t col_idx=0; col_idx<=hight;col_idx++){
		  TM_ILI9341_DrawLine(second_side.x_coords[col_idx], second_side.y_coords[col_idx], third_side.x_coords[col_idx], third_side.y_coords[col_idx],num_swap_16b((uint16_t)color)  );
	  }
  }
}

void draw_SND_logo(uint16_t side,uint16_t x0,uint16_t y0,uint16_t color)
{
	  uint16_t step_w = (uint16_t)((side/2)*sqrt(3));
	  uint16_t step_h = (uint16_t)(side/2);
	  uint16_t hight = side+2*step_h;
	  uint16_t width = 2*step_w;

	  if(x0 > (side/2) && y0>(side/2) && side != 0)
	  {
		  TM_ILI9341_DrawLine(x0-width/2,(uint16_t)((float)y0-side/2),x0,(uint16_t)((float)y0-hight/2),num_swap_16b((uint16_t)color)  ); /*  /   */

		  TM_ILI9341_DrawLine(x0,(uint16_t)((float)y0-hight/2),x0+width/2,(uint16_t)((float)y0-side/2),num_swap_16b((uint16_t)color)  ); /*  \   */

		  TM_ILI9341_DrawLine(x0-width/2,(uint16_t)((float)y0-side/2),x0-width/2,(uint16_t)((float)y0+side/2),num_swap_16b((uint16_t)color)  ); /*  |   */

		  TM_ILI9341_DrawLine(x0+width/2,(uint16_t)((float)y0-side/2),x0+width/2,(uint16_t)((float)y0+side/2),num_swap_16b((uint16_t)color)  ); /*  |   */

		  TM_ILI9341_DrawLine(x0-width/2,(uint16_t)((float)y0+side/2),x0,(uint16_t)((float)y0+hight/2),num_swap_16b((uint16_t)color)  ); /*  \   */

		  TM_ILI9341_DrawLine(x0,(uint16_t)((float)y0+hight/2),x0+width/2,(uint16_t)((float)y0+side/2),num_swap_16b((uint16_t)color)  ); /*  \   */

		  TM_ILI9341_DrawLine(x0 ,(uint16_t)((float)y0-hight/2),x0-side/10,(uint16_t)((float)y0-2*side/3),num_swap_16b((uint16_t)color)  );

		  TM_ILI9341_DrawLine(x0-width/2 ,(uint16_t)((float)y0-side/2),x0-side/10,(uint16_t)((float)y0-2*side/3),num_swap_16b((uint16_t)color)  );

		  ILI9341_draw_unfilled_triangle(x0-width/2, (uint16_t)((float)y0-side/2),x0-side/10,(uint16_t)((float)y0-2*side/3),x0-width/4 , (uint16_t)((float)y0+2*side/10),(uint16_t)color);

		  TM_ILI9341_DrawLine(x0-width/4 ,(uint16_t)((float)y0+2*side/10),x0-width/2,(uint16_t)((float)y0+side/2),num_swap_16b((uint16_t)color)  );

		  ILI9341_draw_unfilled_triangle(x0-width/14, (uint16_t)((float)y0-2*side/3),x0+width/2,(uint16_t)((float)y0-side/2),x0+side/2 , y0,(uint16_t)color);

		  ILI9341_draw_unfilled_triangle(x0-width/4, (uint16_t)((float)y0+2*side/10),x0+side/2,y0,x0 , (uint16_t)((float)y0+hight/2),(uint16_t)color);

		  TM_ILI9341_DrawLine(x0+side/2 ,y0,x0+width/2,(uint16_t)((float)y0+side/2),num_swap_16b((uint16_t)color)  );
	  }
}
