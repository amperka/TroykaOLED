#ifndef __TROYKA_OLED_H__
#define __TROYKA_OLED_H__

#include <Arduino.h>
#include <Wire.h>

#include "./utility/ssd1306.h"

#include "./utility/defaultFonts.h"
#include "./utility/defaultImages.h"

#define OLED_TOP 250
#define OLED_BOTTOM 251
#define OLED_LEFT 252
#define OLED_RIGHT 253
#define OLED_CENTER 254
#define OLED_THIS 255

constexpr int8_t IMG_RAM = 0;
constexpr int8_t IMG_ROM = 1;

constexpr int8_t TXT_CP866 = 0;
constexpr int8_t TXT_UTF8 = 1;
constexpr int8_t TXT_WIN1251 = 2;

constexpr int16_t BLACK = 0;
constexpr int16_t WHITE = 1;
constexpr int16_t INVERSE = 2;

class TroykaOLED {
public:
    TroykaOLED(uint8_t address = 0x3C, uint8_t width = 128, uint8_t heigth = 64);
    void begin(TwoWire* wire = &Wire);
    void update();
    void autoUpdate(bool stateAutoUpdate = true);
    void setBrigtness(uint8_t brigtness);
    void clearDisplay();
    void invertDisplay(bool stateInvert = true);
    void invertText(bool stateInvertText = true);
    void bgText(bool stateTextBG = true);
    void bgImage(bool stateImageBG = true);
    void setFont(const uint8_t* fontData);
    void setCoding(uint8_t codingName = false);
    void setCursor(int numX, int numY);
    void print(char* s, int x = OLED_THIS, int y = OLED_THIS);
    void print(char ch, int x, int y);
    void print(const char* s, int x = OLED_THIS, int y = OLED_THIS);
    void print(String s, int x = OLED_THIS, int y = OLED_THIS);
    void print(int8_t n, int x = OLED_THIS, int y = OLED_THIS, uint8_t base = DEC);
    void print(uint8_t n, int x = OLED_THIS, int y = OLED_THIS, uint8_t base = DEC);
    void print(int16_t n, int x = OLED_THIS, int y = OLED_THIS, uint8_t base = DEC);
    void print(uint16_t n, int x = OLED_THIS, int y = OLED_THIS, uint8_t base = DEC);
    void print(int32_t n, int x = OLED_THIS, int y = OLED_THIS, uint8_t base = DEC);
    void print(uint32_t n, int x = OLED_THIS, int y = OLED_THIS, uint8_t base = DEC);
    void print(double n, int x = OLED_THIS, int y = OLED_THIS, uint8_t digits = 2);
    void drawPixel(int x, int y, uint8_t color = WHITE);
    void drawLine(int x1, int y1, int x2, int y2, uint8_t color = WHITE);
    void drawLine(int x2, int y2, uint8_t color = WHITE);
    void drawRect(int x1, int y1, int x2, int y2, bool fill = false, uint8_t color = WHITE);
    void drawCircle(int x, int y, uint8_t r, bool fill = false, uint8_t color = WHITE);
    void drawImage(const uint8_t* image, int x = OLED_THIS, int y = OLED_THIS, uint8_t mem = IMG_ROM);
    bool getPixel(int x, int y);
    uint8_t getWidth() { return _width; }
    uint8_t getHeigth() { return _height; }
    uint8_t getFontWidth() { return _font.width; }
    uint8_t getFontHeight() { return _font.height; }
    uint8_t getImageWidth(const uint8_t* image, uint8_t mem = IMG_ROM);
    uint8_t getImageHeight(const uint8_t* image, uint8_t mem = IMG_ROM);

private:
    TwoWire* _wire;
    uint8_t _i2cAddress;
    uint8_t _width;
    uint8_t _height;
    uint8_t _numX;
    uint8_t _numY;
    uint8_t _codingName;
    bool _stateInvert;
    bool _stateAutoUpdate;
    bool _stateImageBG;
    uint8_t* _bufferDisplay;
    struct {
        const uint8_t* fontData;
        uint8_t width = 0;
        uint8_t height = 0;
        uint8_t firstSymbol = 0;
        uint8_t sumSymbol = 0;
        uint8_t startSpace[3];
        uint8_t sumSpace[3];
        bool invert = false;
        bool background = true;
        bool setFont = false;
    } _font;

    void _drawPixel(int x, int y, uint8_t color = WHITE);
    void _drawLine(int x1, int y1, int x2, int y2, uint8_t color = WHITE);
    void _print(char*, int x, int y);
    char _itoa(uint8_t num);
    char* _codingCP866(char* data);
    void _sendCommand(uint8_t command);
    void _sendData(uint8_t* data, uint8_t sum);
    void _sendBuffer();
};

#endif // __TROYKA_OLED_H__
