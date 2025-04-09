# tft_lcd_ili9341_hal_dma_stm32
Driving the TFT LCD ILI9341 using HAL,SPI on an STM32 board

#Notes
To whom it may help :-). There so many libraries out there implementing a driver for ILI9341 TFT LCDs
on STM32 boards but not all of them work well. It came out that many test boards bring a counterfeited 
chip, of which initializations parameters differ from those of the original manufacturer. Some modifications had
to be done in TM_ILI9341_InitLCD function in order my LCD to work. All the credits for the legacy code
are presented in the library file. Many thanks to Tilen Majerle(tilen@majerle.eu) for the base code. 


In this rough example,only one SPI bus is used for both touch and printing chips. This limits
the screen update rate since the touch module has a limited SPI speed(around 2Mbps). Please,use separate SPI modules for touch and printing. The actual library is made of tm_stm32f4_ili9341.c,tm_stm32f4_ili9341.h,tm_stm32f4_fonts.c , tm_stm32f4_fonts.h files and some dependencies in spi.c file. DMA didn't work me for some reason :-(. main.c contain a quick example for printing some objects and simple touch menu implementation. my_image_samples.h contains the samples of a test image.  Because I was having some colour being presented wrongly I wrote the num_swap_16b function to swap the colour codes. 

The coordinate system may be positioned not as expected (e.g. flipped around). In such case check function TM_ILI9341_Rotate.

Tips on how to create custom fonts can be found at https://github.com/afiskon/stm32-ssd1306/tree/master/examples/custom-fonts

#Demo

![](https://github.com/Benedito821/tft_lcd_ili9341_hal_dma_stm32/blob/master/welcome_win.gif)
![](https://github.com/Benedito821/tft_lcd_ili9341_hal_dma_stm32/blob/master/menu.gif)