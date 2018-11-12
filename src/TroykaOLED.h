#ifndef TROYKA_OLED_H
#define TROYKA_OLED_H

#include <Arduino.h>
#include <Wire.h>
#include "./utility/defaultFonts.h"
#include "./utility/defaultImages.h"

// адрес дисплея
#define SSD1306_ADDRESS                         0x3C
// установка контрастности, за данной командой должен быть отправлен байт контрастности от 00 до FF (по умолчанию 0x7F)
#define SSD1306_SET_CONTRAST                    0x81
// включить  изображение
#define SSD1306_RAM_ON                          0xA4
// выключить изображение
#define SSD1306_RAM_OFF                         0xA5
// пиксель установленный в 1 будет светиться
#define SSD1306_INVERT_OFF                      0xA6
// пиксель установленный в 1 будет выключен
#define SSD1306_INVERT_ON                       0xA7
// выключить дисплей
#define SSD1306_DISPLAY_OFF                     0xAE
// включить  дисплей
#define SSD1306_DISPLAY_ON                      0xAF
// пустая команда
#define SSD1306_NOP                             0xE3

// включить  прокрутку
#define SSD1306_SCROLL_ON                       0x2F
// выключить  прокрутку
#define SSD1306_SCROLL_OFF                      0x2E
// настройка непрерывной горизонтальной прокрутки вправо. За данной командой должны быть отправлены 6 байт настроек
#define SSD1306_SCROLL_HORIZONTAL_RIGHT         0x26
// настройка непрерывной горизонтальной прокрутки влево. За данной командой должны быть отправлены 6 байт настроек
#define SSD1306_SCROLL_HORIZONTAL_LEFT          0x27
// настройка непрерывной диагональной прокрутки вправо. За данной командой должны быть отправлены 5 байт настроек
#define SSD1306_SCROLL_DIAGONAL_RIGHT           0x29
// настройка непрерывной диагональной прокрутки влево. За данной командой должны быть отправлены 5 байт настроек
#define SSD1306_SCROLL_DIAGONAL_LEFT            0x2A
// настройка непрерывной вертикальной прокрутки. За данной командой должны быть отправлены 2 байта настроек
#define SSD1306_SCROLL_VERTICAL                 0xA3

// установка младшей части адреса колонки на странице
// у данной команды младщие четыре бита должны быть изменены на младшие биты адреса
// комадна предназначена только для режима страничной адресации
#define SSD1306_ADDR_COLUMN_LBS                 0x00
// установка старшей части адреса колонки на странице
// у данной команды младщие четыре бита должны быть изменены на старшие биты адреса
// комадна предназначена только для режима страничной адресации
#define SSD1306_ADDR_COLUMN_MBS                 0x10
// выбор режима адресации 
// за данной командой должен быть отправлен байт младшие биты которого определяют режим:
// 00-горизонтальная (с переходом на новую страницу (строку))
// 01-вертикальная (с переходом на новую колонку) 
// 10-страничная (только по выбранной странице)
#define SSD1306_ADDR_MODE                       0x20
// установка адреса колонки
// за данной командой должны быть отправлены два байта: начальный адрес и конечный адрес
// так можно определить размер экрана в колонках по ширине
#define SSD1306_ADDR_COLUMN                     0x21
// установка адреса страницы
// за данной командой должны быть отправлены два байта: начальный адрес и конечный адрес
// так можно определить размер экрана в строках по высоте
#define SSD1306_ADDR_PAGE                       0x22
// установка номера страницы которая будет выбрана для режима страничной адресации
// у данной команды младщие три бита должны быть изменены на номер страницы
// комадна предназначена только для режима страничной адресации
#define SSD1306_ADDR_ONE_PAGE                   0xB0

// установить начальный адрес ОЗУ (смещение памяти)
// у данной команды младщие шесть битов должны быть изменены на начальный адрес ОЗУ
#define SSD1306_SET_START_LINE                  0x40
// установить режим строчной развёртки справа-налево
#define SSD1306_SET_REMAP_R_TO_L                0xA0
// установить режим строчной развёртки слева-направо
#define SSD1306_SET_REMAP_L_TO_R                0xA1
// установить multiplex ratio (количество используемых выводов COM для вывода данных на дисплей)
// за данной командой должен быть отправлен один байт с указанием количества COM выводов (от 15 до 63)
#define SSD1306_SET_MULTIPLEX_RATIO             0xA8
// установить режим кадровой развёртки снизу-верх
#define SSD1306_SET_REMAP_D_TO_T                0xC0
// установить режим кадровой развёртки сверху-вниз
#define SSD1306_SET_REMAP_T_TO_D                0xC8
// установить смещение дисплея
// за данной командой должен быть отправлен один байт с указанием вертикального сдвига чтения выходов COM (от 0 до 63).
#define SSD1306_SET_DISPLAY_OFFSET              0xD3
// установить тип аппаратной конфигурации COM выводов
// за данной командой должен быть отправлен один байт, у которого:
// четвёртый бит: 0-последовательная / 1-альтернативная 
// пятый бит: 0-отключить COM Left/Right remap / 1-включить COM Left/Right remap
#define SSD1306_SET_COM_PINS                    0xDA

// установить частоту обновления дисплея
// за данной командой должен быть отправлен один байт, старшие четыре бита которого определяют частоту, а младшие делитель
#define SSD1306_SET_DISPLAY_CLOCK               0xD5
// установить фазы DC/DC преобразователя
// за данной командой должен быть отправлен один байт, старшие четыре бита которого определяют период, а младшие период
#define SSD1306_SET_PRECHARGE_PERIOD            0xD9
// установить VcomH (влияет на яркость)
// за данной командой должен быть отправлен один байт, старшие четыре бита которого определяют напряжение
// пример: 0000 - VcomH=0.65Vcc, 0010 - VcomH=0.77Vcc, 0011 - VcomH=0.83Vcc и т.д.
#define SSD1306_SET_VCOM_DESELECT               0xDB
// управление DC/DC преобразователем
// за данной командой должен быть отправлен один байт:
// 0x10 - отключить (VCC подается извне), 0x14 - запустить внутренний DC/DC
#define SSD1306_CHARGE_DCDC_PUMP                0x8D

// положение бита DC в командном байте
// этот бит указывает что следующим байтом будет: 0-команда или 1-данные
#define SSD1306_SHIFT_DC                        0x06
// положение бита CO в командном байте
// этот бит указывает что после следующего байта (команды или данных) будет следовать (если будет): 0-байт данных или 1-новый командный байт
#define SSD1306_SHIFT_CO                        0x07
// (CO=0, DC=0) => 0x00 контрольный байт после которого следует байт команды
#define SSD1306_COMMAND                         (0 << SSD1306_SHIFT_CO) | (0 << SSD1306_SHIFT_DC)
// (CO=0, DC=1) => 0x40 контрольный байт после которого следует байт данных
#define SSD1306_DATA                            (0 << SSD1306_SHIFT_CO) | (1 << SSD1306_SHIFT_DC)

#define OLED_TOP                                250
#define OLED_BOTTOM                             251
#define OLED_LEFT                               252
#define OLED_RIGHT                              253
#define OLED_CENTER                             254
#define OLED_THIS                               255

#define IMG_RAM                                 0
#define IMG_ROM                                 1

#define TXT_CP866                               0
#define TXT_UTF8                                1
#define TXT_WIN1251                             2

#define BLACK                                   0
#define WHITE                                   1
#define INVERSE                                 2

class TroykaOLED {
public:
    TroykaOLED (uint8_t address = 0x3C, uint8_t width = 128, uint8_t heigth = 64);
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
    void print(const char* s, int x = OLED_THIS, int y = OLED_THIS);
    void print(String s , int x = OLED_THIS, int y = OLED_THIS);
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
    uint8_t getFontHeight() {  return _font.height; } 
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
    uint8_t *_bufferDisplay;
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

#endif // TROYKA_OLED_H
