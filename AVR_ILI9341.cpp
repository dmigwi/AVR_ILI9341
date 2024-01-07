/*!
 * @file AVR_ILI9341.cpp (Originally Adafruit_ILI9341.cpp)
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
 * Improved by dmigwi (Daniel Migwi) @2023
 *
 *  @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 */

#include "AVR_ILI9341.h"

#include <limits.h>

/*!
    @brief  Instantiate Adafruit ILI9341 driver with hardware SPI using the
            default SPI peripheral.
    @param  cs   Chip select pin # # (required).
    @param  dc   Data/Command pin # (required).
    @param  rst  Reset pin ## (required).
*/
AVR_ILI9341::AVR_ILI9341(int8_t cs, int8_t dc, int8_t rst)
    : TFT_SPI(cs, dc, rst) {}

// clang-format off
// TFT LCD(ILI9341V) startup configuration is available here: 
// http://www.lcdwiki.com/res/MSP2833_MSP2834/ILI9341V_Init.txt
static const uint8_t PROGMEM initcmd[] = {
  ILI9341_PWCTRA, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
  ILI9341_CMD_CF, 3, 0x00, 0xC1, 0x30,
  ILI9341_TIMCTRA, 3, 0x85, 0x00, 0x78,
  ILI9341_TIMCTRC, 2, 0x00, 0x00,
  ILI9341_POWSEQ, 4, 0x64, 0x03, 0x12, 0x81,
  ILI9341_PUMPRAT, 1, 0x20,
  ILI9341_PWCTR1, 1, 0x23,             // Power control VRH[5:0]
  ILI9341_PWCTR2, 1, 0x10,             // Power control SAP[2:0];BT[3:0]
  ILI9341_VMCTR1, 2, 0x3E, 0x28,       // VCM control - Contrast
  ILI9341_VMCTR2, 1, 0x86,             // VCM control2
  ILI9341_MADCTL, 1, 0x48,             // Memory Access Control
  ILI9341_PIXFMT, 1, 0x55,             // INTERFACE PIXEL FORMAT: 0x66 -> 18 bit; 0x55 -> 16 bit    
  ILI9341_FRMCTR1, 2, 0x00, 0x18,         // VCM Control
  ILI9341_DFUNCTR, 3, 0x08, 0x82, 0x27, // Display Function Control
  ILI9341_INTFCTR, 2, 0x01, 0x30,       // Interface Control //MCU
  ILI9341_EN3GAM, 1, 0x00,                         // 3Gamma Function Disable
  ILI9341_GAMMASET, 1, 0x01,             // Gamma curve selected
  ILI9341_GMCTRP1, 15, 0x0F, 0x35, 0x31, 0x0B, 0x0E, 0x06, 0x49, 0xA7, 0x33, \
     0x07, 0x0F, 0x03, 0x0C, 0x0A, 0x00, // Positive gamma correction 
  ILI9341_GMCTRN1, 15, 0x00, 0x0A, 0x0F, 0x04, 0x11, 0x08, 0x36, 0x58, 0x4D, \
     0x07, 0x10, 0x0C, 0x32, 0x34, 0x0F, // Negative gamma correction
  ILI9341_SLPOUT, 1, 0x80,                // Exit Sleep
  ILI9341_DISPON, 1, 0x80,               // Display On
  ILI9341_NOP,   0,            // shouldn't get executed. End of the List. 
};
// clang-format on

/*!
    @brief   Initialize ILI9341 chip
    Connects to the ILI9341 over SPI and sends initialization procedure commands
    @param    freq  Desired SPI clock frequency
*/
void AVR_ILI9341::begin(uint32_t freq) {
  initSPI(freq);

  SPI_START();

  uint8_t cmd, numArgs;
  const uint8_t *addr = initcmd;

  while ((cmd = pgm_read_byte(addr++)) > 0) {
    // Before Display-On cmd execution there should be a delay for CMD_DELAY.
    if (cmd == ILI9341_DISPON) {
      delay(CMD_DELAY);
    }

    numArgs = pgm_read_byte(addr++);
    sendCommand(cmd, addr, numArgs);
    addr += numArgs;
  }

  SPI_END();
}

/*!
    @brief   Set origin of (0,0) and orientation of TFT display
    @param   m  The index for rotation, from 0-3 inclusive
*/
void AVR_ILI9341::setRotation(uint8_t m) {
  rotation = m % 4;  // can't be higher than 3
  switch (rotation) {
    case 0:
      m = (MADCTL_MX | MADCTL_BGR);
      _width = TFT_WIDTH;
      _height = TFT_HEIGHT;
      break;
    case 1:
      m = (MADCTL_MV | MADCTL_BGR);
      _width = TFT_HEIGHT;
      _height = TFT_WIDTH;
      break;
    case 2:
      m = (MADCTL_MY | MADCTL_BGR);
      _width = TFT_WIDTH;
      _height = TFT_HEIGHT;
      break;
    case 3:
      m = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
      _width = TFT_HEIGHT;
      _height = TFT_WIDTH;
      break;
  }

  SPI_START();
  sendCommand(ILI9341_MADCTL, &m, 1);
  SPI_END();
}

/*!
    @brief   Scroll display memory
    @param   y How many pixels to scroll display by
*/
void AVR_ILI9341::scrollTo(uint16_t y) {
  uint8_t data[2];
  data[0] = y >> 8;
  data[1] = y & 0xff;

  SPI_START();
  sendCommand(ILI9341_VSCRSADD, data, 2);
  SPI_END();
}

/*!
    @brief   Set the height of the Top and Bottom Scroll Margins
    @param   top The height of the Top scroll margin
    @param   bottom The height of the Bottom scroll margin
 */
void AVR_ILI9341::setScrollMargins(uint16_t top, uint16_t bottom) {
  // TFA+VSA+BFA must equal 320
  if (top + bottom <= TFT_HEIGHT) {
    uint16_t middle = TFT_HEIGHT - (top + bottom);
    uint8_t data[6];
    data[0] = top >> 8;
    data[1] = top & 0xff;
    data[2] = middle >> 8;
    data[3] = middle & 0xff;
    data[4] = bottom >> 8;
    data[5] = bottom & 0xff;

    SPI_START();
    sendCommand(ILI9341_VSCRDEF, data, 6);
    SPI_END();
  }
}

/*!
    @brief   Sets the "address window" - the rectangle we will write to RAM with
              the next chunk of SPI data writes. The ILI9341 will automatically
              wrap the data as each row is filled.
    @param   x1  TFT memory 'x' axis origin. Also display's Start Column (SC)
    @param   y1  TFT memory 'y' axis origin. Also display's Start Page (SP)
    @param   x2  TFT memory 'x' axis end point. Also display's End Column (EC)
    @param   y2  TFT memory 'y' axis end point. Also display's End Page (EP)

    @note    The caller should close the SPI bus via SPI_END() command call.
*/
void AVR_ILI9341::setAddressWindow(uint16_t x1, uint16_t y1, uint16_t x2,
                                   uint16_t y2) {
  SPI_START();

  writeCommand(ILI9341_CASET);  // Column address set
  writeData16(x1, 1);
  writeData16(x2, 1);

  writeCommand(ILI9341_PASET);  // Row address set
  writeData16(y1, 1);
  writeData16(y2, 1);

  writeCommand(ILI9341_RAMWR);  // Data memory write command.
}

/*!
    @brief  Reads 8 bits of data from ILI9341 configuration memory.
            NOT from RAM! This is highly undocumented/supported, it's
            really a hack but kinda works?
    @param    commandByte  The command register to read data from
    @param    index  The byte index into the command to read from
    @return   Unsigned 8-bit data read from ILI9341 register
 */
uint8_t AVR_ILI9341::readcommand(uint8_t commandByte, uint8_t index) {
  SPI_START();

  uint8_t data = 0x10 + index;
  sendCommand(0xD9, &data, 1);  // Set Index Register
  uint8_t result = readcommand8(commandByte, index);

  SPI_END();
  return result;
}
