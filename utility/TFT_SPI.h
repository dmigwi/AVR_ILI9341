/*!
 * @file TFT_SPI.h (Originally Adafruit_SPITFT.h)
 *
 * @section Introduction
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
 * Improved by dmigwi (Daniel Migwi)
 *
 *  @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 */

#ifndef _TFT_SPI_H_
#define _TFT_SPI_H_

#include <Print.h>
#include <SPI.h>

#include "Adafruit_GFX.h"

// HARDWARE CONFIG 
// -----------------------------------------------------------------------------

#if defined(ARDUINO_ARCH_AVR)
#define DEFAULT_SPI_FREQ 8000000L  ///< half Hardware SPI default speed
#else
#define DEFAULT_SPI_FREQ 16000000L  ///< Hardware SPI default speed
#endif

// CLASS DEFINITION
// -----------------------------------------------------------------------------

/*!
  @brief  TFT_SPI is an intermediary class between Adafruit_GFX
          and various hardware-specific subclasses for different displays.
          It handles certain operations that are common to a range of
          displays (address window, area fills, etc.).
*/
class TFT_SPI : public Adafruit_GFX {
 public:
  // CONSTRUCTORS
  // ---------------------------------------------------------------------------

  /*!
        @brief Hardware SPI constructor uses the default SPI pin. The default
                MOSI, MISO and SPI pins are used according to the AVR board's
                specification in it variant "pins_arduino.h" file.
        @param width defines the width of the display according to its
                hardware specifications.
        @param height defines the height of the display according to its
                hardware specifications.
        @param cs defines the chipset pin.
        @param dc defines the Data/Command select pin.
        @param rst defines the reset pin of the display.
  */
  TFT_SPI(uint16_t width, uint16_t height, int8_t cs, int8_t dc, int8_t rst);

  // DESTRUCTOR
  // ---------------------------------------------------------------------------

  virtual ~TFT_SPI(){};

  /*!
        @brief Defines the various types of straight line primitives supported.
  */
  enum lineType { Horizontal, Vertical };

  // PUBLIC CLASS MEMBER FUNCTIONS
  // ---------------------------------------------------------------------------

  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void drawLine(int16_t x, int16_t y, int16_t w, lineType line, uint16_t color);
  inline void drawRect(int16_t x, int16_t y, int16_t w, int16_t h,
                       uint16_t color);

  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void drawImage(int16_t x, int16_t y, uint16_t *pcolors, int16_t w, int16_t h);

  void invertDisplay(bool mode);
  uint16_t TFT_SPI::color565(uint8_t red, uint8_t green, uint8_t blue);

  void writeData(uint8_t data);    // Write single byte as DATA
  void writeCommand(uint8_t cmd);  // Write single byte as Command
  void writeData16(uint16_t color, uint32_t len); // Writes 16 bit for provided counts.

  void writeImage(uint8_t *img, uint16_t num);  // Writes image efficiently

 private:
  // PRIVATE CLASS MEMBER FUNCTIONS
  // ---------------------------------------------------------------------------

  uint8_t writeSPI(uint8_t c);

 protected:
  /*!
      @brief  Display-specific initialization function.
      @param  freq  SPI frequency, in hz (or 0 for default or unused).
  */
  virtual void begin(uint32_t freq) = 0;

  /*!
      @brief  Sets up the specific display hardware's "address window"
              for subsequent pixel-pushing operations.
      @param  x  Leftmost pixel of area to be drawn (MUST be within
                 display bounds at current rotation setting).
      @param  y  Topmost pixel of area to be drawn (MUST be within
                 display bounds at current rotation setting).
      @param  w  Width of area to be drawn, in pixels (MUST be >0 and,
                 added to x, within display bounds at current rotation).
      @param  h  Height of area to be drawn, in pixels (MUST be >0 and,
                 added to x, within display bounds at current rotation).
  */
  virtual void setAddressWindow(int16_t x, int16_t y, int16_t w, int16_t h) = 0;

  void initSPI(uint32_t freq = 0, uint8_t spiMode = SPI_MODE0);
  void sendCommand(uint8_t cmd, const uint8_t *dataBytes, uint8_t numBytes);
  uint8_t readcommand8(uint8_t commandByte, uint8_t index);

  void SPI_START();
  void SPI_END();

  // CLASS INSTANCE VARIABLES 
  // ---------------------------------------------------------------------------
#if defined(__cplusplus) && (__cplusplus >= 201100)
  union {
#endif

    struct {           ///<  Values specific to HARDWARE SPI
      SPIClass *_spi;  ///< SPI class pointer

#if defined(SPI_HAS_TRANSACTION)
      SPISettings settings;  ///< SPI transaction settings
#endif
    } hwspi;  ///< Hardware SPI values

#if defined(__cplusplus) && (__cplusplus >= 201100)
  };  ///< Only one interface is active
#endif

  uint8_t connection;  ///< TFT_HARD_SPI, TFT_SOFT_SPI, etc.
  int8_t _rst;         ///< Reset pin # (or -1)
  int8_t _cs;          ///< Chip select pin # (or -1)
  int8_t _dc;          ///< Data/command pin #
  uint8_t invertOnCommand = 0;  ///< Command to enable invert mode
  uint8_t invertOffCommand = 0; ///< Command to disable invert mode
};

#endif  // end _TFT_SPI_H_
