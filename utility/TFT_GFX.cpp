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
 * Improved by dmigwi (Daniel Migwi)  @2024
 *
 *  @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 */

#include "TFT_GFX.h"

/**
 * @brief Class Constructor
 * @param w defines the display width, which is the number of pixels the display
 *          has along x axis.
 * @param h defines the display height, which is the number of pixels the
 *          display has along y axis.
 */
TFT_GFX::TFT_GFX(uint16_t w, uint16_t h) {
  _height = h;  // Value adjusted on Rotation.
  _width = w;   // Value adjusted on Rotation.
  rotation = 0;
}

TFT_GFX::~TFT_GFX() {}

/**
 * @brief Fills the whole screen with the color provided.
 * @param color color pixels to display for the whole viewable area.
 */
void TFT_GFX::fillScreen(uint16_t color) {
  setScreenData(0, 0, _width, _height, color);
}

/**
 * @brief Draws the shape whose valid inputs are confirmed by setting the fill
 *        and stroke colors in the display registers . If empty or out-of-bounds
 *        values are used the shape will not be drawn on the array.
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
 *       if not provided. `strokeColor` pixels will only be drawn if the
 *        `strokewidth` greater than zero was provided. Top-left corner is
 *        assumed to be the corner with coordinates (0,0) on the screen display.
 */
void TFT_GFX::drawShape(uint16_t xAxis, uint16_t yAxis, uint16_t length,
                        uint16_t breadth, uint16_t radius, uint8_t strokeWidth,
                        uint16_t strokeColor, uint16_t fillColor) {
  // Below values used to validate inputs for drawing a specific shape.
  // Drawing single pixel and a line is disabled by default any only enabled if
  // a circle or a rectangle can't be drawn.
  // Drawing a pixel is only activated if no other supported shape can be drawn.
  bool isDrawPixel, isDrawLine;
  bool isDrawCircle = true;
  bool isDrawRect = true;

  // 1. ********* Rectangle Drawing Inputs Validation *************

  // Full rectangle is out-of-bounds.
  if (length > _width || breadth > _height) isDrawRect = false;

  // Neither length nor breadth can be empty.
  if (length == 0 || breadth == 0) isDrawRect = false;

  // 2. ********* Circle Drawing Inputs Validation *************

  uint16_t diameter = 2 * radius;
  uint16_t x0 = diameter + (2 * strokeWidth) + xAxis;
  uint16_t y0 = diameter + (2 * strokeWidth) + yAxis;

  if (radius == 0) isDrawCircle = false;   // radius cannot be empty.
  if (x0 > _width) isDrawCircle = false;   // full circle is out-of-bounds.
  if (y0 > _height) isDrawCircle = false;  // full circle is out-of-bounds.

  // Radius too large compared to breadth or length of the rounded-rectangle.
  if (isDrawRect) {
    if (diameter > length || diameter > breadth) isDrawCircle = false;
  }

  // 3. ********* Line Drawing Inputs Validation *************

  // Only attempt to draw a line if a circle or rectangle can't.
  isDrawLine = !(isDrawCircle || isDrawRect || false);

  // Both length and breadth values cannot be empty.
  if (length == 0 && breadth == 0) isDrawLine = false;

  x0 = length + xAxis;
  if (x0 > _width) isDrawLine = false;  // length out-of-bounds.

  y0 = breadth + yAxis;
  if (y0 > _height) isDrawLine = false;  // breadth out-of-bounds.

  // 4. ********* Pixel Drawing Inputs Validation *************

  isDrawPixel = !(isDrawLine || false);  // Draw a pixel if a line can't.

  // Pixel out of bounds
  if (xAxis > _width || yAxis > _height) isDrawPixel = false;

  uint16_t xFill = 0, xFillCounts = 0;
  uint16_t roundRectLength = length;
  uint16_t roundRectBreadth = breadth;

  if (!isDrawCircle) {
    radius = 0;  // Disable the radius if it was preset by mistake.
    diameter = 0;
  }

  // Set drawing rectangle/rounded-rectangle config if the required valid
  // parameters were found.
  if (isDrawRect) {
    xFill = length;  // Pixels per x axis row to fill at ago.
    roundRectLength -= diameter;
    roundRectBreadth -= diameter;
    xFillCounts = roundRectBreadth;  // Turns to fill pixels per x axis row.
  }

  // Set drawing line config if the required valid parameters were found.
  if (isDrawLine) {
    if (breadth == 0) {  //< Drawing a horizontal line.
      xFill = length;
      xFillCounts = 1;
    } else {  //< Drawing a vertical line.
      xFill = 1;
      xFillCounts = breadth;
    }
  }

  // Set drawing single pixel config if the required valid parameters were
  // found.
  if (isDrawPixel) {
    xFill = 1;        // Pixels per x axis row to fill at ago.
    xFillCounts = 1;  // Turns to fill pixels per x axis row.
  }

  volatile uint16_t yPoint = 0;
  uint16_t xCenter = xAxis + radius;
  uint16_t yCenter = yAxis + radius;

  // Draw stroke shape pixels for the top hemisphere.
  for (uint16_t xPoint = 0; xPoint <= yPoint && isDrawCircle && strokeWidth > 0;
       xPoint++) {
    // Compute the y axis stroke outline value if supported.
    yPoint = circleAlgo(xPoint, radius + strokeWidth);
    plotOctets(Top, xCenter, yCenter, xPoint, yPoint, roundRectLength,
               strokeColor);
  }

  yPoint = 0;  // Reset yPoint;
  // Draw fill shape pixels for the top hemisphere.
  for (uint16_t xPoint = 0; xPoint <= yPoint && isDrawCircle; xPoint++) {
    // Compute the y axis fill outline value.
    yPoint = circleAlgo(xPoint, radius);
    plotOctets(Top, xCenter, yCenter, xPoint, yPoint, roundRectLength,
               fillColor);
  }

  // Draw pixels for the Mid section (rectangle, pixel or line) if enabled.
  if (isDrawRect || isDrawLine || isDrawPixel) {
    setScreenData(xAxis, yAxis + radius, xFill, xFillCounts, fillColor);

    if (strokeWidth > 0) {
      // The rounded-rectangle only requires only two of sides stroked otherwise
      // stroke all the four sides.
      setScreenData(xAxis - strokeWidth - 1, yAxis + radius, strokeWidth,
                    xFillCounts, strokeColor);
      setScreenData(xAxis + length + 1, yAxis + radius, strokeWidth,
                    xFillCounts, strokeColor);

      if (!isDrawCircle) {  // No rounded rectangle
        uint16_t startPos = xAxis + radius - strokeWidth;
        uint16_t strokeLen = length + strokeWidth + strokeWidth;
        setScreenData(startPos, yAxis, strokeLen, strokeWidth, strokeColor);
        setScreenData(startPos - 1, yAxis + breadth, strokeLen + 1, strokeWidth,
                      strokeColor);
      }
    }
  }

  yPoint = 0;  // Reset yPoint;
  // Draw stroke shape pixels for the Bottom hemisphere.
  for (uint16_t xPoint = 0; xPoint <= yPoint && isDrawCircle && strokeWidth > 0;
       xPoint++) {
    // Compute the y axis stroke outline value if supported.
    yPoint = circleAlgo(xPoint, radius + strokeWidth);
    plotOctets(Bottom, xCenter, yCenter + xFillCounts, xPoint, yPoint,
               roundRectLength, strokeColor);
  }

  yPoint = 0;  // Reset yPoint;
  // Draw fill shape pixels for the Bottom hemisphere.
  for (uint16_t xPoint = 0; xPoint <= yPoint && isDrawCircle; xPoint++) {
    // Compute the y axis fill outline value.
    yPoint = circleAlgo(xPoint, radius);
    plotOctets(Bottom, xCenter, yCenter + xFillCounts, xPoint, yPoint,
               roundRectLength, fillColor);
  }
}

/**
 * @brief Plots the all circle's symmetrical octets's fill and outline pixels
 * using coordinates for the single octets computed (xOutline, yOutline).
 * @param hemisphere defines either the top or bottom hemisphere drawn
 *                    on the display.
 * @param xCenter x axis coordinate value for the center of the circle
 * @param yCenter y axis coordinate value for the center of the circle
 * @param xOutline x axis coordinate for the computed circle outline.
 * @param yOutline y axis coordinate for the computed circle outline.
 * @param length length of the rounded-rectangle (= Original Length - Diameter).
 * @param color color pixel used to display the circle fill or stroke.
 *
 * @note The Sketch below shows how the various octets are numbered.
 * @note ****`3 <----> 2`****
 * @note *`4 <---------> 1`*
 * @note *`5 <---------> 8`*
 * @note ****`6 <----> 7`***
 */
void TFT_GFX::plotOctets(segment hemisphere, uint16_t xCenter, uint16_t yCenter,
                         uint16_t xOutline, uint16_t yOutline, uint16_t length,
                         uint16_t color) {
  uint16_t xPos, yPos, xFillPixels;

  switch (hemisphere) {
    case Top:  // Top Hemisphere.
      // Plot Octet 3 <----> 2
      xPos = xCenter - yOutline;
      yPos = yCenter - xOutline;
      xFillPixels = 2 * yOutline + length;
      setScreenData(xPos, yPos, xFillPixels, 1, color);  // draw one line at ago

      // Plot Octet 4 <----> 1
      xPos = xCenter - xOutline;
      yPos = yCenter - yOutline;
      xFillPixels = 2 * xOutline + length;
      setScreenData(xPos, yPos, xFillPixels, 1, color);  // draw one line at ago
      break;

    default:  // Bottom Hemisphere.
              // Plot Octet 5 <----> 8
      xPos = xCenter - xOutline;
      yPos = yCenter + yOutline;
      xFillPixels = 2 * xOutline + length;
      setScreenData(xPos, yPos, xFillPixels, 1, color);  // draw one line at ago

      // Plot Octet 6 <----> 7
      xPos = xCenter - yOutline;
      yPos = yCenter + xOutline;
      xFillPixels = 2 * yOutline + length;
      setScreenData(xPos, yPos, xFillPixels, 1, color);  // draw one line at ago
      break;
  }
}

/**
 * @brief Executes the circle drawing algorithm and return the corresponding
 *        point along the circle edge from it diameter.
 * @param y point along the diameter (or radius) on the y axis.
 * @param radius is actual radius length of the circle.
 * @return Point along the circle circumference that is formed by coordinates
 *          (x,y), where x is the computed value
 * @note The center of the circle is assumed to be at the coordinates (0,0).
 *       Circle equation used is defined here:
 * https://groups.csail.mit.edu/graphics/classes/6.837/F98/Lecture6/circle.html
 */
inline uint16_t TFT_GFX::circleAlgo(uint16_t y, uint16_t radius) {
  float result = (float)((radius * radius) - (y * y));
  return (uint16_t)round(sqrt(result));
}

/**
 * @brief Writes the fill or stroke color values to the display registers.
 * @param xPos defines the x coordinate value on the display grid where
 *              drawing area starts.
 * @param yPos defines the y coordinate value on the display grid where
 *              drawing area starts.
 * @param _xFillPx defines the number of pixels along the x axis to filled with
 *                the `_fillColor`.
 * @param _depth defines the number of rows (along y axis) where the `_xFillPx`
 *                pixels will be editted.
 * @param _fillcolor is the fill color of the shape.
 *
 * @note `Display start column` = xPos;
 * @note `Display end column` = _xFillPx + xPos + (_strokePx * 2);
 * @note `Display start page` = yPos;
 * @note `Display end page` = _depth + yPos;
 */
void TFT_GFX::setScreenData(uint16_t xPos, uint16_t yPos, uint16_t _xFillPx,
                            uint8_t _depth, uint16_t _fillcolor) {
  // Set the drawing area.
  setAddressWindow(xPos, yPos, _xFillPx + xPos, _depth + yPos);

  // Write fill color data to the registers
  writeData16(_fillcolor, (_xFillPx + 1) * _depth);
}