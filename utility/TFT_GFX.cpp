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

TFT_GFX::TFT_GFX(uint16_t w, uint16_t h) {
    _width = w;
    _height = h;
    _pixels = w*h;

    // Sets the screen to display color black(0x00) before loading anything else.
    displayData[_pixels] = {};
}

TFT_GFX::~TFT_GFX() {}

/**
 * @brief Fills the whole screen with the color provided.
 * @param color color pixels to display for the whole viewable area.
*/
void TFT_GFX::fillScreen(uint16_t color) {
    memset(&displayData, color, _pixels);
}

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
void TFT_GFX::drawShape(uint16_t xAxis, uint16_t yAxis,
                        uint16_t length, uint16_t breadth, uint16_t radius,
                        uint8_t strokeWidth, uint16_t strokeColor,
                        uint16_t fillColor) {
  // Below values used to validate inputs for drawing a specific shape.
  // Drawing single pixel and a line is disabled by default any only enabled if
  // A circle or rectangle can't be drawn.
  // Drawing a pixel is only activated if no other supported shape can be drawn.
  bool isDrawPixel, isDrawLine;
  bool isDrawCircle = true;
  bool isDrawRect = true;

  // 1. ********* Rectangle Drawing Inputs Validation *************

  if (length > _width || breadth > _height) {
    isDrawRect = false;    // full rectangle is out-of-bounds.
    isDrawCircle = false;  // Rounded-edges rectangle cannot be drawn.
  }

  if (length == 0 || breadth == 0) {
    isDrawRect = false;    // Either of the length or breadth cannot be empty.
    isDrawCircle = false;  // Rounded-edges rectangle cannot be drawn.
  }

  // 2. ********* Circle Drawing Inputs Validation *************

  volatile uint16_t diameter = 2 * radius;
  volatile uint16_t x0 = diameter + (2 * strokeWidth) + xAxis;
  volatile uint16_t y0 = diameter + (2 * strokeWidth) + yAxis;

  if (x0 > _width) isDrawCircle = false;   // full circle is out-of-bounds.
  if (y0 > _height) isDrawCircle = false;  // full circle is out-of-bounds.

  if (radius == 0) isDrawCircle = false;  // radius cannot be empty.

  // Radius too large compared to breadth or length of the rounded-rectangle
  if (diameter > length || diameter > breadth) isDrawCircle = false;

  if (radius > xAxis || radius > yAxis) {
    isDrawCircle = false;  // a circle sector (part of it) is out-of-bounds.
  }

  // 3. ********* Line Drawing Inputs Validation *************

  // Only attempt to draw a line if a circle or reectangle can't.
  isDrawLine = isDrawCircle && isDrawRect && false;

  // No length or breadth found.
  if (length == 0 && breadth == 0) isDrawLine = false;

  x0 = length + xAxis;
  if (x0 > _width) isDrawLine = false;  // length out-of-bounds.

  y0 = breadth + yAxis;
  if (y0 > _height) isDrawLine = false;  // breadth out-of-bounds.

  // 4. ********* Pixel Drawing Inputs Validation *************

  isDrawPixel = isDrawLine && false;  // Draw a pixel if a line can't.

  // Pixel out of bounds
  if (xAxis > _width || yAxis > _height) isDrawPixel = false;

  volatile uint16_t xFill, xStroke;

  // Draw a circle if the required valid parameters were found.
  if (isDrawCircle) {
    int y = 0;
    while (y <= xFill) {  // True only till the first octet is plotted.
      y++;

      // Compute and plot the x strokePixels value if supported.
      if (strokeWidth > 0) {
        xStroke = circleAlgo(y, radius + strokeWidth);
        plotOctets(xAxis, yAxis, xStroke, y, length-diameter, breadth-diameter, strokeColor);
      }

      // Compute and plot the x fillPixels value.
      xFill = circleAlgo(y, radius);
      plotOctets(xAxis, yAxis, xFill, y, length-diameter, breadth-diameter, fillColor);
    }
  }

  volatile uint16_t startFillPos, xFillCounts;
  if (isDrawRect) {
    xFill = length;         // Pixels per x axis row to fill at ago.
    xFillCounts = breadth - diameter;  // Turns to fill pixels per x axis row.
  }

  if (isDrawLine) {
    if (breadth == 0) {  //< Drawing a horizontal line.
      xFill = length;
      xFillCounts = 1;
    } else {  //< Drawing a vertical line.
      xFill = 1;
      xFillCounts = breadth;
    }
    radius = 0; // Disable the radius if it was preset.
  }

  if (isDrawPixel) {
    xFill = 1;        // Pixels per x axis row to fill at ago.
    xFillCounts = 1;  // Turns to fill pixels per x axis row.
    radius = 0; // Disable the radius if it was preset.
  }

  while (xFillCounts > 0) {
     startFillPos = _width * (xAxis - xFill - radius) + xFill + 1;
    memset(&displayData[startFillPos], fillColor, xFill);
    xFillCounts--;
  }
}

/**
 * @brief Plots the all circle's symmetrical octets's fill and outline pixels in
 * the displayData array using coordinates for the single octets calculated
 * (xFill, yFill).
 * @param xAxis x value for moving the shape into the required position.
 * @param yAxis y value for moving the shape into the required position
 * @param xFill x coordinates for the circle outline.
 * @param yFill y coordinates for the circle outline.
 * @param length length of the rounded-rectangle (= Original L - 2 * Radius).
 * @param breadth width of the rounded-rectangle. (= Original W - 2 * Radius).
 * @param color color pixel used to display the circle fill or outline.
 *
 * @note The Sketch below shows how the various octets are numbered.
 * @note ****`3 <----> 2`****
 * @note *`4 <---------> 1`*
 * @note *`5 <---------> 8`*
 * @note ****`6 <----> 7`***
 */
void TFT_GFX::plotOctets(uint16_t xAxis, uint16_t yAxis, uint16_t xFill,
                         uint16_t yFill, uint16_t length, uint16_t breadth,
                         uint16_t color) {
  volatile uint16_t startFillPos, xFillPixels;

  // Plot Octet 3 <----> 2
  xFillPixels = 2 * yFill + length;
  startFillPos = _width * (xAxis - xFill) + xFill + 1;
  memset(&displayData[startFillPos], color, xFillPixels);

  // Plot Octet 4 <----> 1
  xFillPixels = 2 * xFill + length;
  startFillPos = _width * (yAxis - yFill) + yFill + 1;
  memset(&displayData[startFillPos], color, xFillPixels);

  // Plot Octet 5 <----> 8
  xFillPixels = 2 * xFill + length;
  startFillPos = _width * (yAxis + yFill + breadth) + yFill + 1;
  memset(&displayData[startFillPos], color, xFillPixels);

  // Plot Octet 6 <----> 7
  xFillPixels = 2 * yFill + length;
  startFillPos = _width * (xAxis + xFill + breadth) + xFill + 1;
  memset(&displayData[startFillPos], color, xFillPixels);
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