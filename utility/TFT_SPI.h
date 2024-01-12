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
 * Improved by dmigwi (Daniel Migwi)  @2023
 *
 *  @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 */

#ifndef _TFT_SPI_H_
#define _TFT_SPI_H_

#include <Print.h>
#include <SPI.h>

#include "TFT_GFX.h"

// HARDWARE CONFIG
// -----------------------------------------------------------------------------

#if defined(ARDUINO_ARCH_AVR)
#define DEFAULT_SPI_FREQ 8000000L  ///< half Hardware SPI default speed
#else
#define DEFAULT_SPI_FREQ 16000000L  ///< Hardware SPI default speed
#endif

#define TFT_WIDTH 240     ///< Maximum TFT display hardware width.
#define TFT_HEIGHT 320    ///< Maximum TFT display hardware height.
#define TFT_PIXELS 76800  ///< Maximum TFT display pixels. (=240 x 320)

// CLASS DEFINITION
// -----------------------------------------------------------------------------

/*!
  @brief  TFT_SPI is an intermediary class between TFT_GFX and various
  hardware-specific subclasses for different displays. It handles certain
  operations that are common to a range of displays (address window, area fills,
  etc.).
*/
class TFT_SPI : public TFT_GFX {
 public:
  // CONSTRUCTORS
  // ---------------------------------------------------------------------------

  /*!
        @brief Hardware SPI constructor uses the default SPI pin. The default
                MOSI, MISO and SPI pins are used according to the AVR board's
                specification in it variant "pins_arduino.h" file.
        @param cs defines the chipset pin.
        @param dc defines the Data/Command select pin.
        @param rst defines the reset pin of the display.
  */
  TFT_SPI(int8_t cs, int8_t dc, int8_t rst);

  // DESTRUCTOR
  // ---------------------------------------------------------------------------

  virtual ~TFT_SPI(){};

  // PUBLIC CLASS MEMBER FUNCTIONS
  // ---------------------------------------------------------------------------

  // void drawScreen(uint16_t xAxis, uint16_t yAxis, uint16_t width,
  //                 uint16_t height, uint16_t startFillPos);

 private:
  // PRIVATE CLASS MEMBER FUNCTIONS
  // ---------------------------------------------------------------------------

  uint8_t writeSPI(uint8_t c);

 protected:
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
  virtual void setAddressWindow(uint16_t x, uint16_t y, uint16_t w,
                                uint16_t h) = 0;
  void writeData(uint8_t data);    // Write single byte as DATA
  void writeCommand(uint8_t cmd);  // Write single byte as Command
  void writeData16(uint16_t color,
                   uint32_t len);  // Writes 16 bit for provided counts.

  void writeImage(uint16_t *img, uint32_t num);  // Writes image efficiently

  void initSPI(uint32_t freq = 0, uint8_t spiMode = SPI_MODE0);
  void sendCommand(uint8_t cmd, const uint8_t *dataBytes, uint8_t numBytes);
  uint8_t readcommand8(uint8_t commandByte, uint8_t index);

  // void setScreenData(uint16_t startPos, uint16_t color, uint16_t num);

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

  int8_t _rst;  ///< Reset pin # (or -1)
  int8_t _cs;   ///< Chip select pin # (or -1)
  int8_t _dc;   ///< Data/command pin #

  uint16_t WIDTH;
  uint16_t HEIGHT;
// 
  // uint16_t screenData[TFT_PIXELS];
};

#endif  // end _TFT_SPI_H_
