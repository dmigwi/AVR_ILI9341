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

#define PIXELS 76800  // = 240 x 230

enum Shape { Pixel, Line, Circle, Rectangle };

class TFT_GFX {
 public:
  TFT_GFX(uint16_t w, uint16_t h);  // Constructor

  ~TFT_GFX();  // Destructor

  void drawShape(Shape sType, uint16_t xAxis, uint16_t yAxis, uint16_t length,
                 uint16_t breadth, uint16_t radius, uint8_t strokeWidth,
                 uint16_t strokeColor, uint16_t fillColor);

 protected:
  uint16_t _width, _height;

 private:
  uint16_t displayData[PIXELS];

  /**
   * @brief Defines one-forth of an equally divided circle from its center.
   *        The quadrants are labelled clockwise starting with the top-right
   *        quadrant as q1.
   */
  enum quadrant { q1, q2, q3, q4 };

  virtual void writeImage(uint8_t *img, uint16_t num);

  void drawPixel(uint16_t xAxis, uint16_t yAxis, uint16_t breadth,
                 uint16_t fillColor);
  void drawLine(uint16_t xAxis, uint16_t yAxis, uint16_t length,
                uint16_t breadth, uint16_t fillColor);
  void drawCircle(uint16_t xAxis, uint16_t yAxis, uint16_t radius,
                  uint8_t strokeWidth, uint16_t strokeColor,
                  uint16_t fillColor);
  void drawRect(uint16_t xAxis, uint16_t yAxis, uint16_t length,
                uint16_t breadth, uint16_t radius, uint8_t strokeWidth,
                uint16_t strokeColor, uint16_t fillColor);
  void drawQuadrant(quadrant q, uint16_t x0, uint16_t y0, uint16_t radius,
                    uint8_t strokeWidth, uint16_t strokeColor,
                    uint16_t fillColor);
};
