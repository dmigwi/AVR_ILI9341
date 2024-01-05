/*!
 * @file TFT_GFX.cpp
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

#include "TFT_GFX.h"

TFT_GFX::TFT_GFX(uint16_t w, uint16_t h) {}

TFT_GFX::~TFT_GFX() {}

/**
 * @brief Draws the supported shapes by setting the respective the colors in the
 *        displayData array. If empty or out-of-bounds values are used the
 *         shape will not be drawn on the array.
 * @param sType Name the supported shapes as defined in Shape enum.
 *              (Required for: All Shapes)
 * @param xAxis x coordinate for the top-left corner where the shape drawing
 *             will begin from. (Required for: All Shapes)
 * @param yAxis y coordinate for the top-left corner where the shape drawing
 *             will begin from. (Required for: All Shapes)
 * @param length distance along x axis from the top-left corner where the
 *              shape will occupy. (Required for: Rectangles & Horizontal lines)
 * @param breadth distance along y axis from the top-left corner where the
 *              shape will occupy. (Required for: Rectangles & Vertical lines)
 * @param radius distance from a central point where the circumference outline
 *              will be plotted at. (Required for Circle and Rounded Rectanges)
 * @param strokeWidth size of the shape ouline. Default is zero. Can only apply
 *              on Circles, Rectanges and Lines.
 * @param strokeColor color pixel used to display the shape outline (stroke)
 *                    on the display. Its is updated on displayData array.
 * @param fillColor color pixel used to display the actual shape.
 * @note `fillColor` and `strokeColor` pixels defaults to zero (color BLACK)
 *       if not provided. `strokeColor` pixels will only be drawn if
 *        `strokewidth` greater than zero was provided. Top-left corner is
 *        assumed to be the corner with coordinates (0,0) on the screen display.
 */
void TFT_GFX::drawShape(Shape sType, uint16_t xAxis, uint16_t yAxis,
                        uint16_t length, uint16_t breadth, uint16_t radius,
                        uint8_t strokeWidth, uint16_t strokeColor,
                        uint16_t fillColor) {
  // If invalid x and y axis points used return;
  if (xAxis >= _width - 1 || yAxis >= _width - 1) return;
  
  switch (sType) {
    case Shape::Pixel:
      drawPixel(xAxis, yAxis, breadth, fillColor);
      break;

    case Shape::Line:
      drawLine(xAxis, yAxis, length, breadth, fillColor);
      break;

    case Shape::Circle:
      drawCircle(xAxis, yAxis, radius, fillColor, strokeWidth, strokeColor);
      break;

    case Shape::Rectangle:
      drawRect(xAxis, yAxis, length, breadth, radius, strokeWidth, strokeColor,
               fillColor);
      break;

    default:
      // Unsupported shape was found.
      return;
  }
}

void TFT_GFX::drawPixel(uint16_t xAxis, uint16_t yAxis, uint16_t breadth,
                        uint16_t fillColor) {}
void TFT_GFX::drawLine(uint16_t xAxis, uint16_t yAxis, uint16_t length,
                       uint16_t breadth, uint16_t fillColor) {}
void TFT_GFX::drawRect(uint16_t xAxis, uint16_t yAxis, uint16_t length,
                       uint16_t breadth, uint16_t radius, uint8_t strokeWidth,
                       uint16_t strokeColor, uint16_t fillColor) {}

/**
 * @brief Draws a circle by setting the shape fill and outline (if provided)
 *          pixels in the displayData array.
 * @param xAxis x coordinate for the top-left corner where the Circle drawing
 *             will begin from. (Required)
 * @param yAxis y coordinate for the top-left corner where the Circle drawing
 *             will begin from. (Required)
 * @param radius distance from a central point where the circumference outline
 *              will be plotted at. (Required)
 * @param strokeWidth size of the Circle ouline. Default is zero. (Optional)
 * @param strokeColor color pixel used to display the circle outline (stroke)
 *                    on the display. Its is updated on displayData array.
 * @param fillColor color pixel used to display the actual circle fill.
 * @note The center of the circle is adjusted so that the square area within
 *        which the circle will fit can be computed from the top-left
 *        corner referred to by the coordinates (xAxis, yAxis).
 */
void TFT_GFX::drawCircle(uint16_t xAxis, uint16_t yAxis, uint16_t radius,
                         uint8_t strokeWidth, uint16_t strokeColor,
                         uint16_t fillColor) {
  volatile uint16_t x0 = (2*radius) + (2*strokeWidth) + xAxis;
  if (x0 > _width) return; // circle exceeds the display bounds

   volatile uint16_t y0 = (2*radius) + (2*strokeWidth) + yAxis;
  if (y0 > _height) return; // circle exceeds the display bounds

  x0 = xAxis + radius; // Set the correct x axis center point.
  y0 = yAxis + radius; // Set the correct y axis center point.

  // Draw quadrant 1
  TFT_GFX::drawQuadrant(q1, x0, y0, radius, strokeWidth, strokeColor,
                        fillColor);
  // Draw quadrant 2
  TFT_GFX::drawQuadrant(q2, x0, y0, radius, strokeWidth, strokeColor,
                        fillColor);
  // Draw quadrant 3
  TFT_GFX::drawQuadrant(q3, x0, y0, radius, strokeWidth, strokeColor,
                        fillColor);
  // Draw quadrant 4
  TFT_GFX::drawQuadrant(q4, x0, y0, radius, strokeWidth, strokeColor,
                        fillColor);
}

/**
 * @brief Plots the quadrant's fill and outline pixels in the displayData array.
 * @param q quadrant type either:; q1, q2, q3 or q4.
 * @param x0 x coordinate for the center of the circle.
 * @param y0 y coordinate for the center of the circle.
 * @param radius is actual radius length of the circle.
 * @param strokeWidth size of the Circle ouline.
 * @param strokeColor color pixel used to display the circle outline.
 * @param fillColor color pixel used to display the actual circle fill.
 * @note While computing the pixels using the circle algorithm, center (0,0) is
 * used but is then adjusted with provided parameters (xAxis, yAxis) before
 * updating displayData array.
 */
void TFT_GFX::drawQuadrant(TFT_GFX::quadrant q, uint16_t x0, uint16_t y0,
                           uint16_t radius, uint8_t strokeWidth,
                           uint16_t strokeColor, uint16_t fillColor) {
  volatile uint16_t startFillPos, startStrokePos, xPixels, xFillPixels,
      xStrokePixels;
  volatile float strokeToFillPxs;

  if (strokeWidth > 0) strokeToFillPxs = (float)strokeWidth / (float)radius;

  for (int i = 0; i < radius; i++) {
    xPixels = circleAlgo(i, radius + strokeWidth);

    // Compute the strokePixels
    if (strokeWidth > 0) xStrokePixels = (uint16_t)(strokeToFillPxs * xPixels);

    // Compute the fill pixels.
    xFillPixels = xPixels - xStrokePixels;

    switch (q) {
      case q1:
        startFillPos = x0 - (i * _width);
        if (strokeWidth > 0) startStrokePos = startFillPos + xFillPixels;
        break;

      case q2:
        startFillPos = x0 + (i * _width);
        if (strokeWidth > 0) startStrokePos = startFillPos + xFillPixels;
        break;

      case q3:
        startFillPos = x0 - (i * _width) - xFillPixels;
        if (strokeWidth > 0) startStrokePos = startFillPos - xStrokePixels;
        break;

      default:  // q4
        startFillPos = x0 + (i * _width) - xFillPixels;
        if (strokeWidth > 0) startStrokePos = startFillPos - xStrokePixels;
        break;
    }
    // Set the circle fill
    memset(&displayData[startFillPos], fillColor, xFillPixels);
    if (strokeWidth > 0)
      memset(&displayData[startStrokePos], strokeColor, xStrokePixels);
  }
}

/**
 * @brief Executes the circle drawing algorithm to return a point along the
 *        the circle edge from it diameter (or radius).
 * @param y point along the diameter (or radius) on the y axis.
 * @param radius is actual radius length of the circle.
 * @return Point along the circle circumference that is formed by coordinates
 *          (x,y), where x is the computed value
 * @note The center of the circle is assumed to be at the coordinates (0,0).
 *       Circle equation used is defined here:
 * https://groups.csail.mit.edu/graphics/classes/6.837/F98/Lecture6/circle.html
 */
inline uint16_t circleAlgo(uint16_t y, uint16_t radius) {
  volatile float result = (float)((radius * radius) - (y * y));
  return (uint16_t)sqrt(result);
}