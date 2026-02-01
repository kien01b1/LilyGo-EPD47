/**
 * @copyright Copyright (c) 2024  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2024-04-05
 * @note      Arduino Setting
 *            Tools ->
 *                  Board:"ESP32S3 Dev Module"
 *                  USB CDC On Boot:"Enable"
 *                  USB DFU On Boot:"Disable"
 *                  Flash Size : "16MB(128Mb)"
 *                  Flash Mode"QIO 80MHz
 *                  Partition Scheme:"16M Flash(3M APP/9.9MB FATFS)"
 *                  PSRAM:"OPI PSRAM"
 *                  Upload Mode:"UART0/Hardware CDC"
 *                  USB Mode:"Hardware CDC and JTAG"
 *  
 */

#ifndef BOARD_HAS_PSRAM
#error "Please enable PSRAM, Arduino IDE -> tools -> PSRAM -> OPI !!!"
#endif

/* Simple firmware for a ESP32 displaying a static image on an EPaper Screen.
 *
 * Write an image into a header file using a 3...2...1...0 format per pixel,
 * for 4 bits color (16 colors - well, greys.) MSB first.  At 80 MHz, screen
 * clears execute in 1.075 seconds and images are drawn in 1.531 seconds.
 */

#include <Arduino.h>
#include "epd_driver.h"
#include "utilities.h"
#include <SPI.h>
#include <SD.h>

uint8_t *framebuffer;
#define FILE_SYSTEM SD
#define DBG_OUTPUT_PORT Serial
File bmpFile;

// Read 16-bit little-endian
uint16_t read16(File &f) {
  uint16_t result;
  result  = f.read();
  result |= f.read() << 8;
  return result;
}

// Read 32-bit little-endian
uint32_t read32(File &f) {
  uint32_t result;
  result  = f.read();
  result |= f.read() << 8;
  result |= f.read() << 16;
  result |= f.read() << 24;
  return result;
}

void setup()
{
    //init screen
    Serial.begin(115200);

    epd_init();

    framebuffer = (uint8_t *)heap_caps_malloc(EPD_WIDTH * EPD_HEIGHT / 2, MALLOC_CAP_SPIRAM);

    if (!framebuffer) {
        DBG_OUTPUT_PORT.println("alloc memory failed !!!");
        while (1)
            ;
    }

    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
    epd_poweron();
    epd_clear();
    //draw something here bitch
    epd_poweroff();
    //screen init completed
    //init the SD card
    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    bool rlst = FILE_SYSTEM.begin(SD_CS, SPI);
    if (rlst) {
        DBG_OUTPUT_PORT.println("FS initialized.");
        
    } else {
        DBG_OUTPUT_PORT.println("FS initialization failed.");
    }
    //sd card init completed
    // read the file
    bmpFile = SD.open("/Itachi_9.bmp");
    if (!bmpFile) {
       DBG_OUTPUT_PORT.println("Failed to open BMP");
       return;
    }
}

void update(uint32_t delay_ms)
{
    epd_poweron();
    epd_clear();
    volatile uint32_t t1 = millis();
    //render blank screen
    epd_draw_grayscale_image(epd_full_screen(), framebuffer);
    volatile uint32_t t2 = millis();
    DBG_OUTPUT_PORT.printf("EPD draw took %dms.\r\n", t2 - t1);
    epd_poweroff();
    delay(delay_ms;)
}

void loop()
{

}

