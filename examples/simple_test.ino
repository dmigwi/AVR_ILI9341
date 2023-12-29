
#include <SPI.h>
#include <AVR_ILI9341.h>

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
// If using the breakout, change pins as desired

void setup() {

  Serial.begin(9600);
  delay(3000);

  pinMode(BACKLIGHT, OUTPUT);
  digitalWrite(BACKLIGHT, HIGH);

  tft.begin(8000000);
  delay(1000);

  Serial.println(F("========Start!=========="));

  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand(ILI9341_RDDST);
  Serial.print(F("Display Status : 0x"));
  Serial.print(x, HEX);  Serial.print("=="); Serial.println(x, BIN);
  // x = tft.readcommand(ILI9341_RDDST, 1);
  // Serial.print(F("Display Status 1: 0x"));
  // Serial.println(x, HEX);
  // x = tft.readcommand(ILI9341_RDDST, 2);
  // Serial.print(F("Display Status 2: 0x"));
  // Serial.println(x, HEX);
  // x = tft.readcommand(ILI9341_RDDST, 3);
  // Serial.print(F("Display Status 3: 0x"));
  // Serial.println(x, HEX);
  // x = tft.readcommand(ILI9341_RDDST, 4);
  // Serial.print(F("Display Status 4: 0x"));
  // Serial.println(x, HEX);
  // x = tft.readcommand(ILI9341_RDDST, 5);
  // Serial.print(F("Display Status 5: 0x"));
  // Serial.println(x, HEX);
  x = tft.readcommand(ILI9341_RDMODE);
  Serial.print(F("Display Power Mode: 0x"));
  Serial.print(x, HEX);  Serial.print("=="); Serial.println(x, BIN);
  x = tft.readcommand(ILI9341_RDMADCTL);
  Serial.print(F("MADCTL Mode: 0x"));
  Serial.print(x, HEX);  Serial.print("=="); Serial.println(x, BIN);
  x = tft.readcommand(ILI9341_RDPIXFMT);
  Serial.print(F("Pixel Format: 0x"));
  Serial.print(x, HEX);  Serial.print("=="); Serial.println(x, BIN);
  x = tft.readcommand(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x");
  Serial.print(x, HEX);  Serial.print("=="); Serial.println(x, BIN);
  x = tft.readcommand(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x");
  Serial.print(x, HEX);  Serial.print("=="); Serial.println(x, BIN);

  Serial.println(F("Benchmark                Time (microseconds)"));
  delay(50);
  // Serial.print(F("Screen fill              "));
  // Serial.println(testFillScreen());
  // delay(50);
  Serial.print(F("Text                     "));
  Serial.println(testText());


  delay(3000);
  Serial.println(F("=======Done!========"));
}

void loop(void) {
  tft.fillScreen(ILI9341_RED);
  for (int x=20; x<50; x++) {
    for (int y=20; y<50; y++) {
        tft.drawPixel(x, y, ILI9341_YELLOW);
    }
  }
  digitalWrite(TFT_DC, LOW);
  delay(10000);
  // for (uint8_t rotation = 0; rotation < 4; rotation++) {
  //   tft.setRotation(rotation);
  //   testText();
  //   delay(1000);
  // }
}

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
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  return micros() - start;
}
