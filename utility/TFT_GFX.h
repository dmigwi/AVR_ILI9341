/*!
 * @file TFT_GFX.h
 *
 * @section intro_sec Introduction
 *
 * This file is part AVR_ILI9341 library package files. It is an implementation
 * of the TFT Display using the chipset ILI9341V and been optimised mainly
 * for Leonardo and Mega 2560 boards. It may work with other AVR boards but
 * that cannot be guaranteed.
 *
 * @section author Author
 *
 * Originally written by Limor "ladyada" Fried for Adafruit Industries,
 * with contributions from the open source community.
 * Improved by dmigwi (Daniel Migwi)  @2023
 *
 *  @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 */

#include "Arduino.h"
#include "TFT_SPI.h"

class TFT_GFX {
 public:
  TFT_GFX(uint16_t w, uint16_t h, uint16_t pixels);  // Constructor

  ~TFT_GFX();  // Destructor

  void fillScreen(uint16_t color);

  void drawShape(uint16_t xAxis, uint16_t yAxis, uint16_t length,
                 uint16_t breadth, uint16_t radius, uint8_t strokeWidth,
                 uint16_t strokeColor, uint16_t fillColor);

 protected:
  uint16_t _width, _height, _pixels;
  uint8_t rotation;

 private:
  void plotOctets(uint16_t xAxis, uint16_t yAxis, uint16_t xFill,
                  uint16_t yFill, uint16_t length, uint16_t breadth,
                  uint16_t color);
 virtual void setScreenData(uint16_t startPos, uint16_t color, uint16_t len);
};
