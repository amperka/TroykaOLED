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

enum class Encoding {
    CP866,
    UTF8,
    CP1251  
};

class TroykaOLED {
public:
    TroykaOLED(uint8_t address = 0x3C, uint8_t width = HORIZONTAL_SIZE, uint8_t height = VERTICAL_SIZE);
    void begin(TwoWire* wire = &Wire);
    void update();
    void updateAll();
    void autoUpdate(bool stateAutoUpdate = true);
    void setBrightness(uint8_t brightness);
    void clearDisplay();
    void invertDisplay(bool stateInvert = true);
    void invertText(bool stateInvertText = true);
    void textColor(uint8_t color = WHITE);
    void imageColor(uint8_t color = WHITE);
    void setFont(const uint8_t* fontData);
    void setCoding(Encoding codingName);
    void setCursor(int16_t numX, int16_t numY);
    void print(char character, int16_t x = OLED_THIS, int16_t y = OLED_THIS);
    void print(char* line, int16_t x = OLED_THIS, int16_t y = OLED_THIS);
    void print(const char* line, int16_t x = OLED_THIS, int16_t y = OLED_THIS);
    void print(String s, int16_t x = OLED_THIS, int16_t y = OLED_THIS);
    void print(int8_t n, int16_t x = OLED_THIS, int16_t y = OLED_THIS, uint8_t base = DEC);
    void print(uint8_t n, int16_t x = OLED_THIS, int16_t y = OLED_THIS, uint8_t base = DEC);
    void print(int16_t n, int16_t x = OLED_THIS, int16_t y = OLED_THIS, uint8_t base = DEC);
    void print(uint16_t n, int16_t x = OLED_THIS, int16_t y = OLED_THIS, uint8_t base = DEC);
    void print(int32_t n, int16_t x = OLED_THIS, int16_t y = OLED_THIS, uint8_t base = DEC);
    void print(uint32_t n, int16_t x = OLED_THIS, int16_t y = OLED_THIS, uint8_t base = DEC);
    void print(double n, int16_t x = OLED_THIS, int16_t y = OLED_THIS, uint8_t digits = 2);
    void drawPixel(int16_t x, int16_t y, uint8_t color = WHITE);
    void drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color = WHITE);
    void drawLine(int16_t x2, int16_t y2, uint8_t color = WHITE);
    void drawRect(int16_t x1, int16_t y1, int16_t x2, int16_t y2, bool fill = false, uint8_t color = WHITE);
    void drawCircle(int16_t x, int16_t y, uint8_t r, bool fill = false, uint8_t color = WHITE);
    void drawImage(const uint8_t* image, int16_t x = OLED_THIS, int16_t y = OLED_THIS, uint8_t mem = IMG_ROM);
    uint8_t getPixel(int16_t x, int16_t y);
    uint8_t getWidth() { return _width; }
    uint8_t getHeight() { return _height; }
    uint8_t getFontWidth() { return _font.width; }
    uint8_t getFontHeight() { return _font.height; }
    uint8_t getImageWidth(const uint8_t* image, uint8_t mem = IMG_ROM);
    uint8_t getImageHeight(const uint8_t* image, uint8_t mem = IMG_ROM);

    // deprecated methods for compatibility
    void setBrigtness(uint8_t brigtness) { setBrightness(brigtness); }
    void bgText(bool stateTextBG = true) { textColor(stateTextBG); }
    void bgImage(bool stateImageBG = true) { imageColor(stateImageBG); }

private:
    TwoWire* _wire;
    uint8_t _i2cAddress;
    uint8_t _width;
    uint8_t _height;

    struct {
        int16_t x;
        int16_t y;
    } _last;

    Encoding _codingName;
    bool _stateInvert;
    bool _stateAutoUpdate;
    uint8_t _imageColor;

    union {
        uint64_t column[HORIZONTAL_SIZE];
        uint8_t asBytes[(VERTICAL_SIZE >> 3) * HORIZONTAL_SIZE];
    } _screenBuffer;

    struct {
        const uint8_t* data;
        const uint8_t* remap;
        uint8_t width = 0;
        uint8_t height = 0;
        bool invert = false;
        uint8_t color = WHITE;
        bool setFont = false;
    } _font;

    struct {
        int16_t left;
        int16_t right;
    } _changedColumns;

    void _drawPixel(int16_t x, int16_t y, uint8_t color = WHITE);
    void _drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color = WHITE);
    void _interpretParameters(int16_t x, int16_t y, int16_t w, int16_t h);
    uint8_t _punchDigits(uint32_t n, char* d, uint8_t p, uint8_t b);
    void _radixConverter(uint32_t num, bool sign, uint8_t bits, uint8_t base, int16_t x, int16_t y);
    uint8_t _charToGlyph(char c) const;
    void _print(char c, int16_t x, int16_t y);
    void _print(char* s, int16_t x, int16_t y);
    char _itoa(uint8_t num);
    char* _encodeToCP866(uint8_t* data);
    void _sendCommand(uint8_t command);
    void _sendCommand(uint8_t command, uint8_t value);
    void _sendCommand(uint8_t command, uint8_t value1, uint8_t value2);
    void _sendData(uint8_t* data, uint8_t sum);
    void _stamp(int16_t x, int16_t y, uint64_t body, uint8_t color);
    void _sendBuffer();
    void _markChangedColumns(int16_t l, int16_t r);
    void _sendColumns(uint8_t start, uint8_t end);
};

#endif // __TROYKA_OLED_H__
