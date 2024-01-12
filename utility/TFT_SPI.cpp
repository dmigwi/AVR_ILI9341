/*!
 * @file TFT_SPI.cpp (Originally Adafruit_SPITFT.cpp)
 *
 * @section intro_sec Introduction
 *
 * This file is part AVR_ILI9341 library package files. It is an implementation
 * of the TFT Display using the chipset ILI9341V and been optimised mainly
 * for Leonardo and Mega 2560 boards. It may work with other AVR boards but
 * that cannot be guaranteed.
 *
 *  @section dependencies Dependencies
 *
 * This library depends on <a href="https://github.com/adafruit/Adafruit_GFX">
 * Adafruit_GFX</a> being present on your system. Please make sure you have
 * installed the latest version before using this library.
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

#include "TFT_SPI.h"

#include <pins_arduino.h>

/*!
    @brief   TFT_SPI constructor for hardware SPI using a specific
             SPI peripheral.
    @param   w         Display width in pixels at default rotation (0).
    @param   h         Display height in pixels at default rotation (0).
    @param   cs        Arduino pin # for chip-select (-1 if unused, tie CS low).
    @param   dc        Arduino pin # for data/command select (required).
    @param   rst       Arduino pin # for display reset (optional, display reset
                       can be tied to MCU reset, default of -1 means unused).
    @note    Output pins are not initialized in constructor; application
             typically will need to call subclass' begin() function, which
             in turn calls this library's initSPI() function to initialize
             pins.
    @note TFT_WIDTH defines the width of the display according to its
              hardware specifications.
    @note TFT_HEIGHT defines the height of the display according to its
                hardware specifications.
*/
TFT_SPI::TFT_SPI(int8_t cs, int8_t dc, int8_t rst)
    : TFT_GFX(TFT_WIDTH, TFT_HEIGHT, TFT_PIXELS) {
  _rst = rst;
  _cs = cs;
  _dc = dc;
  WIDTH = TFT_WIDTH;    // Constant throughout the program lifetime
  HEIGHT = TFT_HEIGHT;  // Constant throughout the program lifetime

  hwspi._spi = &SPI;  // Pointer to SPIClass type.
}

// CLASS MEMBER FUNCTIONS --------------------------------------------------

/*!
    @brief  Configure microcontroller pins for TFT interfacing. Typically
            called by a subclass' begin() function.
    @param  freq     SPI frequency when using hardware SPI. If default (0)
                     is passed, will fall back on a device-specific value.
                     Value is ignored when using software SPI or parallel
                     connection.
    @param  spiMode  SPI mode when using hardware SPI. MUST be one of the
                     values SPI_MODE0, SPI_MODE1, SPI_MODE2 or SPI_MODE3
                     defined in SPI.h. Do NOT attempt to pass '0' for
                     SPI_MODE0 and so forth...the values are NOT the same!
                     Use ONLY the defines! (Pity it's not an enum.)
    @note   Another anachronistically-named function; this is called even
            when the display connection is parallel (not SPI). Also, this
            could probably be made private...quite a few class functions
            were generously put in the public section.
*/
void TFT_SPI::initSPI(uint32_t freq, uint8_t spiMode) {
  if (!freq) freq = DEFAULT_SPI_FREQ;  // If no freq specified, use default

  // Init basic control pins common to all connection types
  pinMode(_cs, OUTPUT);
  pinMode(_dc, OUTPUT);
  pinMode(_rst, OUTPUT);

  // This are the same default pins defined in pins_arduino.h file
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(SCK, OUTPUT);

  pinMode(_dc, HIGH);  // Data mode set
  pinMode(_cs, HIGH);  // Chipset disabled

  hwspi._spi->begin();
#if defined(SPI_HAS_TRANSACTION)
  hwspi.settings = SPISettings(
      freq, MSBFIRST, spiMode);  // 8000000 gives max speed on AVR 16MHz
#else
  hwspi._spi->setClockDivider(freq);  // 4 MHz (half speed)
  hwspi._spi->setBitOrder(MSBFIRST);
  hwspi._spi->setDataMode(spiMode);
#endif

  if (_rst >= 0) {
    // Toggle _rst low to reset
    digitalWrite(_rst, HIGH);
    delay(200);
    digitalWrite(_rst, LOW);
    delay(200);
    digitalWrite(_rst, HIGH);
    delay(200);
  }
}

/*!
    @brief Initiates the SPI transaction if supported, to gain exclusive access
          to the SPI bus. Lastly it activates the chip select pin in that order
*/
void TFT_SPI::SPI_START(void) {
#if defined(SPI_HAS_TRANSACTION)
  hwspi._spi->beginTransaction(hwspi.settings);
#endif

  digitalWrite(_cs, LOW);
}

/*!
    @brief Disables the chip select pin before releasing the access to the
          SPI bus for others to use.
*/
void TFT_SPI::SPI_END(void) {
  digitalWrite(_cs, HIGH);

#if defined(SPI_HAS_TRANSACTION)
  hwspi._spi->endTransaction();
#endif
}

/*!
    @brief  Does the actual writing of 8-bit Data or Command. To write a command
            chipset pin must set to low and otherwise when sending data.
    @param c 8-bit Data/Command to be executed.
    @returns an output if any exists. To read the output registers, the NOP
              should be used.
*/
uint8_t TFT_SPI::writeSPI(uint8_t c) { return hwspi._spi->transfer(c); }

/*!
    @brief  Sets the data transfer mode to Command, Activates the chip-select
   pin, initiates the SPI interface before sending the command. Writes a single
   8-bit command to the display After the command is sent, the chip-select pin
   is disabled and the SPI interface shutdown. DC set to LOW.
    @param  cmd  8-bit command to write.
*/
void TFT_SPI::writeCommand(uint8_t cmd) {
  digitalWrite(_dc, LOW);

  writeSPI(cmd);
}

/*!
    @brief  Sets the data transfer mode to Data, Activates the chip-select pin,
            initiates the SPI interface before sending the data.
            Writes a single 8-bit Data to the display
            After the data is sent, the chip-select pin is disabled and the SPI
            interface shutdown. DC set to HIGH.
    @param  d8  8-bit Data to write.
*/
void TFT_SPI::writeData(uint8_t d8) {
  digitalWrite(_dc, HIGH);

  writeSPI(d8);
}

/*!
   @brief  Sets the data transfer mode to Data, then writes a the 16-bit Data
            to the display memory unit for provided pixels count. Data tranfer
            for each 16-bit data happens as two consecutive data write requests
            with the most significant bytes bieng sent last. DC set to HIGH.
    @param  color  16-bit pixel color in '565' RGB format.
    @param  num   Number of pixels to draw.
*/
void TFT_SPI::writeData16(uint16_t color, uint32_t num) {
  digitalWrite(_dc, HIGH);

  while (num > 0) {
    writeSPI(color >> 8);
    writeSPI(color);
    num--;
  }
}

/*!
    @brief Similar to writeData16() but is optimised for pointers.
    @param  img  16-bit pixel color location pointer.
    @param  num   Number of pixels to draw.
    @note This is a fast method to send multiple 16-bit values from RAM via SPI
*/
void TFT_SPI::writeImage(uint16_t *img, uint32_t num) {
  digitalWrite(_dc, HIGH);
  uint16_t color;

  while (num > 0) {
    color = *img;
    writeSPI(color >> 8);
    writeSPI(color);
    num--;
  }
}

// /**
//  * @brief  Draws 16-bit display color pixels (565 RGB) in the screenData array
//  *from the start position specified (xAxis,yAxis) position till the length
//  *specified.
//  * @param  xAxis    x axis value used to move the shape into the position.
//  * @param  yAxis    y axis value used to move the shape into the position.
//  * @param  width    Width of display that will have its pixels updated.
//  * @param  height   Height of display that will have its pixels updated.
//  * @param  startFillPos  screenData index array from where pixels will be
//  *editted.
//  * @note All parameters take up default values of zero. If width and height
//  *parameters have zero values, they are assigned the max display
//  *width and height measurements indicating that the whole screen should be
//  *redrawn.
//  */
// void TFT_SPI::drawScreen(uint16_t xAxis, uint16_t yAxis, uint16_t width,
//                          uint16_t height, uint16_t startFillPos) {
//   if (width == 0) width = _width;     // Max display width is assigned.
//   if (height == 0) height = _height;  // Max display height is assigned.

//   if (xAxis >= _width || yAxis >= _height || width > _width || height > _height)
//     return;
//   setAddressWindow(xAxis, yAxis, (xAxis + width - 1), (yAxis + height - 1));

//   writeImage(&screenData[startFillPos], width * height);

//   SPI_END();
// }

// /***
//  * @brief Makes updates on the screen display data array.
//  * @param startPos An array index from which pixels to be displayed are held
//  * till the need to display the data arises.
//  * @param color the color pixel to update from the specified array locations.
//  * @param num count of pixels to be affected by the current color changes.
//  */
// void TFT_SPI::setScreenData(uint16_t startPos, uint16_t color, uint16_t num) {
//   memset(&screenData[startPos], color, num);
// }

// -------------------------------------------------------------------------
// Miscellaneous class member functions that don't draw anything.

/*!
      @brief  Handles the complete sending of 8-bit commands and data chunks.
              It does not initiate or close the SPI communication session. This
              should be managed by its' caller. This is done to increase its
              efficiency on cases when multiple simultaneous calls need to be
   executed.
      @param   cmd  The command byte
      @param   dataBytes  A pointer to the Data bytes to send
      @param   numBytes  The number of bytes we should send
*/
void TFT_SPI::sendCommand(uint8_t cmd, const uint8_t *dataBytes,
                          uint8_t numBytes) {
  writeCommand(cmd);  // Set commmand mode and execute the command input.

  for (uint8_t i = 0; i < numBytes; i++) {
    uint8_t data = pgm_read_byte(dataBytes++);
    writeData(data);  // Set data mode and send the data bytes
  }
}

/*!
    @brief   Read 8 bits of data from display configuration memory (not RAM).
            It does not initiate or close the SPI communication session. This
            should be managed by its' caller. This is done to increase its
            efficiency on cases when multiple simultaneous calls need to be
   executed. This is highly undocumented/supported and should be avoided,
            function is only included because some of the examples use it. Uses
            NOP (0x00) instruction to read command output.
    @param   commandByte The command register to read data from.
    @param   index The byte index into the command to read from.
    @return  Unsigned 8-bit data read from display register.
 */
uint8_t TFT_SPI::readcommand8(uint8_t commandByte, uint8_t index) {
  writeCommand(commandByte);

  pinMode(_dc, HIGH);  // Data Mode; should not be used often.
  uint8_t result;

  do {
    result = writeSPI(0x00);  // query the NOP instruction.
  } while (index--);          // Discard bytes up to index'th

  return result;
}
