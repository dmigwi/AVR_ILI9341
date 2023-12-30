
#include <AVR_ILI9341.h>
#include <SPI.h>

// For the Adafruit shield, these are the default.
// helps to write each wire colour
#define TFT_DC 8   // green
#define TFT_RST 9  // yellow
#define TFT_CS 10  // orange

#define BACKLIGHT 4  // black

// Pins for Leonardo board are the leonardo ICPS pins in "pins_arduino.h" file.
// #define TFT_MISO 14  // white
// #define TFT_SCK 15   // purple
// #define TFT_MOSI 16  // blue

// Use hardware SPI and the above for CS,DC and RST pins
AVR_ILI9341 tft = AVR_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(19200);
  delay(3000);

  pinMode(BACKLIGHT, OUTPUT);
  digitalWrite(BACKLIGHT, HIGH);

  tft.begin(8000000);
  delay(1000);

  Serial.println(F("========Start!=========="));

  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand(ILI9341_RDDST);
  Serial.print(F("Display Status : 0x"));
  Serial.print(x, HEX);
  Serial.print("==");
  Serial.println(x, BIN);
  x = tft.readcommand(ILI9341_RDMODE);
  Serial.print(F("Display Power Mode: 0x"));
  Serial.print(x, HEX);
  Serial.print("==");
  Serial.println(x, BIN);
  x = tft.readcommand(ILI9341_RDMADCTL);
  Serial.print(F("MADCTL Mode: 0x"));
  Serial.print(x, HEX);
  Serial.print("==");
  Serial.println(x, BIN);
  x = tft.readcommand(ILI9341_RDPIXFMT);
  Serial.print(F("Pixel Format: 0x"));
  Serial.print(x, HEX);
  Serial.print("==");
  Serial.println(x, BIN);
  x = tft.readcommand(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x");
  Serial.print(x, HEX);
  Serial.print("==");
  Serial.println(x, BIN);
  x = tft.readcommand(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x");
  Serial.print(x, HEX);
  Serial.print("==");
  Serial.println(x, BIN);

  Serial.println(F("Benchmark                Time (microseconds)"));
  delay(50);
  Serial.print(F("Screen fill              "));
  Serial.println(testFillScreen());
  delay(50);
  Serial.print(F("Text                     "));
  Serial.println(testText());

  delay(3000);
  Serial.println(F("=======Done!========"));
}

void loop(void) {}

unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(ILI9341_BLACK);
  yield();
  tft.fillScreen(ILI9341_RED);
  yield();
  tft.fillScreen(ILI9341_GREEN);
  yield();
  tft.fillScreen(ILI9341_BLUE);
  yield();
  tft.fillScreen(ILI9341_BLACK);
  yield();
  return micros() - start;
}

unsigned long testText() {
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.println();
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_RED);
  tft.println("Display Testing:");
  tft.println();
  tft.setTextColor(ILI9341_GREEN);
  tft.println("The quick brown fox jumps over the lazy dog");
  tft.println();
  tft.println("THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG'S BACK");
  tft.println();
  tft.println("1234567890");
  tft.println();
  tft.println(",@#$%^&*()_-'';][}{\|`!?<>");
  tft.println();
  tft.setTextColor(ILI9341_YELLOW);
  tft.println("@dmigwi");
  return micros() - start;
}
