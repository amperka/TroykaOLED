#ifndef __TROYKA_OLED_H__
#define __TROYKA_OLED_H__

#include <Arduino.h>
#include <Wire.h>

#include "./utility/ssd1306.h"

#include "./utility/defaultFonts.h"
#include "./utility/defaultImages.h"

constexpr int16_t OLED_TOP = 32762;
constexpr int16_t OLED_BOTTOM = 32763;
constexpr int16_t OLED_LEFT = 32764;
constexpr int16_t OLED_RIGHT = 32765;
constexpr int16_t OLED_CENTER = 32766;
constexpr int16_t OLED_THIS = 32767;

constexpr int8_t IMG_RAM = 0;
constexpr int8_t IMG_ROM = 1;

constexpr int8_t TXT_CP866 = 0;
constexpr int8_t TXT_UTF8 = 1;
constexpr int8_t TXT_WIN1251 = 2;

constexpr int16_t BLACK = 0;
constexpr int16_t WHITE = 1;
constexpr int16_t INVERSE = 2;

constexpr uint8_t HORIZONTAL_SIZE = 128;
constexpr uint8_t VERTICAL_SIZE = 64;

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
    void drawPixel(int16_t x, int16_t y, uint8_t color = WHITE);
    void drawLine(int x1, int y1, int x2, int y2, uint8_t color = WHITE);
    void drawLine(int x2, int y2, uint8_t color = WHITE);
    void drawRect(int x1, int y1, int x2, int y2, bool fill = false, uint8_t color = WHITE);
    void drawCircle(int x, int y, uint8_t r, bool fill = false, uint8_t color = WHITE);
    void drawImage(const uint8_t* image, int x = OLED_THIS, int y = OLED_THIS, uint8_t mem = IMG_ROM);
    uint8_t getPixel(int16_t x, int16_t y);
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

    union {
        uint64_t column[HORIZONTAL_SIZE];
        uint8_t asBytes[(VERTICAL_SIZE >> 3) * HORIZONTAL_SIZE];
    } _screenBuffer;

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

    void _drawPixel(int16_t x, int16_t y, uint8_t color = WHITE);
    void _drawLine(int x1, int y1, int x2, int y2, uint8_t color = WHITE);
    void _print(char*, int x, int y);
    char _itoa(uint8_t num);
    char* _codingCP866(char* data);
    void _sendCommand(uint8_t command);
    void _sendCommand(uint8_t command, uint8_t value);
    void _sendCommand(uint8_t command, uint8_t value1, uint8_t value2);
    void _sendData(uint8_t* data, uint8_t sum);
    void _stamp(int16_t x, int16_t y, uint64_t body, uint8_t color);
    void _sendBuffer();
};

#endif // __TROYKA_OLED_H__
