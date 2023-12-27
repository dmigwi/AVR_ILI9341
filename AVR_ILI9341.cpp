// See rights and use declaration in license.txt
// This was originally Adafruit_ILI9341 library but has been modified(optimised) for the AVR (Leonardo and Mega) arduino boards.
// The hardware SPI pins are used for communication with the screen.
// @dmigwi 2023
//
// BSD license as below and no warranty terms applies

/*!
 * @file Adafruit_ILI9341.h
 *
 * This is the documentation for Adafruit's ILI9341 driver for the
 * Arduino platform.
 *
 * This library works with the Adafruit 2.8" Touch Shield V2 (SPI)
 *    http://www.adafruit.com/products/1651
 * Adafruit 2.4" TFT LCD with Touchscreen Breakout w/MicroSD Socket - ILI9341
 *    https://www.adafruit.com/product/2478
 * 2.8" TFT LCD with Touchscreen Breakout Board w/MicroSD Socket - ILI9341
 *    https://www.adafruit.com/product/1770
 * 2.2" 18-bit color TFT LCD display with microSD card breakout - ILI9340
 *    https://www.adafruit.com/product/1770
 * TFT FeatherWing - 2.4" 320x240 Touchscreen For All Feathers
 *    https://www.adafruit.com/product/3315
 *
 * These displays use SPI to communicate, 4 or 5 pins are required
 * to interface (RST is optional).
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 *
 * This library depends on <a href="https://github.com/adafruit/Adafruit_GFX">
 * Adafruit_GFX</a> being present on your system. Please make sure you have
 * installed the latest version before using this library.
 *
 * Written by Limor "ladyada" Fried for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */


#include "AVR_ILI9341.h"
// #ifndef ARDUINO_STM32_FEATHER
// #include "pins_arduino.h"
// #ifndef RASPI
// #include "wiring_private.h"
// #endif
// #endif
#include <limits.h>

#if defined(ARDUINO_ARCH_AVR)
#define SPI_DEFAULT_FREQ 8000000 ///< 8000000 gives max speed on AVR 16MHz
#else
#define SPI_DEFAULT_FREQ 16000000 ///< Default SPI data clock frequency
#endif

#define MADCTL_MY 0x80  ///< Bottom to top
#define MADCTL_MX 0x40  ///< Right to left
#define MADCTL_MV 0x20  ///< Reverse Mode
#define MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define MADCTL_MH 0x04  ///< LCD refresh right to left

#define SPI_INTERFACES_COUNT 1

/**************************************************************************/
/*!
    @brief  Instantiate Adafruit ILI9341 driver with hardware SPI using the
            default SPI peripheral.
    @param  cs   Chip select pin # (OK to pass -1 if CS tied to GND).
    @param  dc   Data/Command pin # (required).
    @param  rst  Reset pin # (optional, pass -1 if unused).
*/
/**************************************************************************/
AVR_ILI9341::AVR_ILI9341(int8_t cs, int8_t dc, int8_t rst)
    : Adafruit_SPITFT(ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT, cs, dc, rst) {}

// clang-format off
static const uint8_t PROGMEM initcmd[] = {
  ILI9341_CMD_EF, 3, 0x03, 0x80, 0x02,
  ILI9341_CMD_CF, 3, 0x00, 0xC1, 0x30,
  ILI9341_POWSEQ, 4, 0x64, 0x03, 0x12, 0x81,
  ILI9341_TIMCTRA, 3, 0x85, 0x00, 0x78,
  ILI9341_PWCTRA, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
  ILI9341_PUMPRAT, 1, 0x20,
  ILI9341_TIMCTRC, 2, 0x00, 0x00,
  ILI9341_PWCTR1, 1, 0x23,             // Power control VRH[5:0]
  ILI9341_PWCTR2, 1, 0x10,             // Power control SAP[2:0];BT[3:0]
  ILI9341_VMCTR1, 2, 0x3e, 0x28,       // VCM control
  ILI9341_VMCTR2, 1, 0x86,             // VCM control2
  ILI9341_MADCTL, 1, 0x48,             // Memory Access Control
  ILI9341_VSCRSADD, 1, 0x00,             // Vertical scroll zero
  ILI9341_PIXFMT, 1, 0x55,
  ILI9341_FRMCTR1, 2, 0x00, 0x18,
  ILI9341_DFUNCTR, 3, 0x08, 0x82, 0x27, // Display Function Control
  ILI9341_EN3GAM, 1, 0x00,                         // 3Gamma Function Disable
  ILI9341_GAMMASET, 1, 0x01,             // Gamma curve selected
  ILI9341_GMCTRP1, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
  ILI9341_GMCTRN1, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma
  ILI9341_SLPOUT, 1, 0x80,                // Exit Sleep
  ILI9341_DISPON, 1, 0x80,                // Display on
  ILI9341_NOP,                   // shouldn't get executed. End of the List. 
};
// clang-format on

/**************************************************************************/
/*!
    @brief   Initialize ILI9341 chip
    Connects to the ILI9341 over SPI and sends initialization procedure commands
    @param    freq  Desired SPI clock frequency
*/
/**************************************************************************/
void AVR_ILI9341::begin(uint32_t freq) {

  if (!freq)
    freq = SPI_DEFAULT_FREQ;
  initSPI(freq);

//   if (_rst < 0) {                 // If no hardware reset pin...
//     sendCommand(ILI9341_SWRESET); // Engage software reset
//     delay(150);
//   }

  SPI_START();
  CS_ACTIVE();

  uint8_t cmd, x, numArgs;
  const uint8_t *addr = initcmd;

  while ((cmd = pgm_read_byte(addr++)) > 0) {
    x = pgm_read_byte(addr++);
    numArgs = x & 0x7F;
    sendCommand(cmd, addr, numArgs);
    addr += numArgs;
    if (x & 0x80)
      delay(150);
  }

  // Trigger Horizontal rotation.
  AVR_ILI9341::setRotation(0);

  CS_IDLE();
  SPI_END();

  _width = ILI9341_TFTWIDTH;
  _height = ILI9341_TFTHEIGHT;
}

/**************************************************************************/
/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void AVR_ILI9341::setRotation(uint8_t m) {
  rotation = m % 4; // can't be higher than 3
  switch (rotation) {
  case 0:
    m = (MADCTL_MX | MADCTL_BGR);
    _width = ILI9341_TFTWIDTH;
    _height = ILI9341_TFTHEIGHT;
    break;
  case 1:
    m = (MADCTL_MV | MADCTL_BGR);
    _width = ILI9341_TFTHEIGHT;
    _height = ILI9341_TFTWIDTH;
    break;
  case 2:
    m = (MADCTL_MY | MADCTL_BGR);
    _width = ILI9341_TFTWIDTH;
    _height = ILI9341_TFTHEIGHT;
    break;
  case 3:
    m = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
    _width = ILI9341_TFTHEIGHT;
    _height = ILI9341_TFTWIDTH;
    break;
  }

  SPI_START();
  CS_ACTIVE();

  sendCommand(ILI9341_MADCTL, &m, 1);

  CS_IDLE();
  SPI_END();
}

/**************************************************************************/
/*!
    @brief   Enable/Disable display color inversion
    @param   invert True to invert, False to have normal color
*/
/**************************************************************************/
void AVR_ILI9341::invertDisplay(bool invert) {
  Adafruit_SPITFT::invertDisplay(invert);
}

/**************************************************************************/
/*!
    @brief   Scroll display memory
    @param   y How many pixels to scroll display by
*/
/**************************************************************************/
void AVR_ILI9341::scrollTo(uint16_t y) {
  uint8_t data[2];
  data[0] = y >> 8;
  data[1] = y & 0xff;

  SPI_START();
  CS_ACTIVE();

  sendCommand(ILI9341_VSCRSADD, data, 2);

  CS_IDLE();
  SPI_END();
}

/**************************************************************************/
/*!
    @brief   Set the height of the Top and Bottom Scroll Margins
    @param   top The height of the Top scroll margin
    @param   bottom The height of the Bottom scroll margin
 */
/**************************************************************************/
void AVR_ILI9341::setScrollMargins(uint16_t top, uint16_t bottom) {
  // TFA+VSA+BFA must equal 320
  if (top + bottom <= ILI9341_TFTHEIGHT) {
    uint16_t middle = ILI9341_TFTHEIGHT - (top + bottom);
    uint8_t data[6];
    data[0] = top >> 8;
    data[1] = top & 0xff;
    data[2] = middle >> 8;
    data[3] = middle & 0xff;
    data[4] = bottom >> 8;
    data[5] = bottom & 0xff;

    SPI_START();
    CS_ACTIVE();

    sendCommand(ILI9341_VSCRDEF, data, 6);

    CS_IDLE();
    SPI_END();
  }
}

/**************************************************************************/
/*!
    @brief   Set the "address window" - the rectangle we will write to RAM with
   the next chunk of SPI data writes. The ILI9341 will automatically wrap
   the data as each row is filled.
    @param   x1  TFT memory 'x' axis origin.
    @param   y1  TFT memory 'y' axis origin.
    @param   x2  TFT memory 'x' axis end point.
    @param   y1  TFT memory 'y' axis end point.
*/
/**************************************************************************/
void AVR_ILI9341::setAddrWindow(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
    SPI_START();
    CS_ACTIVE();

    x1 = min(x1, _width-1); // Convert unsigned in to signed.
    x2 = min(x2, _width-1);

    DC_COMMAND();  writeSPI(ILI9341_CASET);    // Column address set
    DC_DATA();  writeColor(x1, 1);  writeColor(x2, 1);

    y1 = min(y1, _height-1);
    y2 = min(y2, _height-1);

    DC_COMMAND(); writeSPI(ILI9341_PASET); // Row address set
    DC_DATA(); writeColor(y1, 1); writeColor(y2, 1);
    
    DC_COMMAND(); writeSPI(ILI9341_RAMWR);

    // DC_DATA();
    // no CS_IDLE + SPI_END, DC_DATA to save memory
}

/**************************************************************************/
/*!
    @brief  Reads 8 bits of data from ILI9341 configuration memory. NOT from RAM!
            This is highly undocumented/supported, it's really a hack but kinda
   works?
    @param    commandByte  The command register to read data from
    @param    index  The byte index into the command to read from
    @return   Unsigned 8-bit data read from ILI9341 register
 */
/**************************************************************************/
uint8_t AVR_ILI9341::readcommand(uint8_t commandByte, uint8_t index) {
  SPI_START();
  CS_ACTIVE();

  uint8_t data = 0x10 + index;
  sendCommand((uint8_t)0xD9, &data, (uint8_t)1); // Set Index Register
  uint8_t result = readcommand8(commandByte);

  CS_IDLE();
  SPI_END();
  return result;
}
