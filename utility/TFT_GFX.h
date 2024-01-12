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
 * Improved by dmigwi (Daniel Migwi)  @2024
 *
 *  @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 */

#ifndef _TFT_GFX_H_
#define _TFT_GFX_H_

#include "Arduino.h"

/**
 * @brief Defines a circle's subdivision into Top and Bottom hemispheres.
 */
enum segment { Top, Bottom };

class TFT_GFX {
 public:
  TFT_GFX(uint16_t w, uint16_t h);  // Constructor

  ~TFT_GFX();  // Destructor

  virtual void writeData16(uint16_t color, uint32_t num) = 0;
  virtual void setAddressWindow(uint16_t x, uint16_t y, uint16_t w,
                                uint16_t h) = 0;

  void fillScreen(uint16_t color);

  void drawShape(uint16_t xAxis, uint16_t yAxis, uint16_t length,
                 uint16_t breadth, uint16_t radius, uint8_t strokePixels,
                 uint16_t strokeColor, uint16_t fillColor);

 protected:
  uint16_t _width, _height;
  uint8_t rotation;

 private:
  uint16_t circleAlgo(uint16_t y, uint16_t radius);
  void plotOctets(segment hemisphere, uint16_t xCenter, uint16_t yCenter,
                  uint16_t xOutline, uint16_t yOutline, uint16_t length,
                  uint16_t color);
  void setScreenData(uint16_t xPos, uint16_t yPos, uint16_t _xFillPx,
                     uint8_t _depth, uint16_t _fillcolor);
};
#endif  // end _TFT_GFX_H_
