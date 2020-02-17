/****************************************************************************/
//  Function:       Cpp file for TroykaOLED
//  Hardware:       SSD1306
//  Arduino IDE:    Arduino-1.8.5
//  Author:         Igor Dementiev
//  Date:           NOV 10,2018
//  Version:        v1.0.0
//  by www.amperka.ru
/****************************************************************************/

#include "TroykaOLED.h"

#define _AUTO_UPDATE()      \
    if (_stateAutoUpdate) { \
        update();           \
    }

TroykaOLED::TroykaOLED(uint8_t i2cAddress, uint8_t width, uint8_t height) {
    _i2cAddress = i2cAddress;
    _width = width;
    _height = height;
    _stateInvert = false;
    _stateAutoUpdate = false;
    _imageColor = WHITE;
    _codingName = TXT_UTF8;
}

void TroykaOLED::begin(TwoWire* wire) {
    _wire = wire;
    // инициируем I²C
    _wire->begin();
    // выключаем дисплей
    _sendCommand(SSD1306_DISPLAY_OFF);
    // устанавливаем частоту обновления дисплея в значение 0x80 (по умолчанию)
    _sendCommand(SSD1306_SET_DISPLAY_CLOCK, 0x80);
    // устанавливаем multiplex ratio (коэффициент мультиплексирования COM выводов)
    // в значение 0x3F (по умолчанию)
    _sendCommand(SSD1306_SET_MULTIPLEX_RATIO, 0x3f);
    // устанавливаем смещение дисплея в 0 (без смещения)
    _sendCommand(SSD1306_SET_DISPLAY_OFFSET, 0);
    // устанавливаем смещение ОЗУ в значение 0 (без смещения)
    _sendCommand(SSD1306_SET_START_LINE | 0);
    // настраиваем схему питания (0x14 - включить внутренний DC-DC
    // преобразователь, 0x10 - отключить внутренний DC/DC)
    _sendCommand(SSD1306_CHARGE_DCDC_PUMP, 0x14);
    // устанавливаем режим автоматической адресации (0x00-горизонтальная,
    // 0x01-вертикальная, 0x10-страничная)
    _sendCommand(SSD1306_ADDR_MODE, 0x01);
    // устанавливаем режим строчной развертки (слева/направо)
    _sendCommand(SSD1306_SET_REMAP_L_TO_R);
    // устанавливаем режим кадровой развертки (сверху/вниз)
    _sendCommand(SSD1306_SET_REMAP_T_TO_D);
    // устанавливаем аппаратную конфигурация COM выводов в значение 0x12 (по
    // умолчанию)
    _sendCommand(SSD1306_SET_COM_PINS, 0x12);
    // устанавливаем контрастность в значение 0xCF (допустимы значения от 0x00 до
    // 0xFF)
    _sendCommand(SSD1306_SET_CONTRAST, 0xff);
    // настраиваем схему DC/DC преобразователя (0xF1 - Vcc снимается с DC/DC
    // преобразователя, 0x22 - Vcc подается извне)
    _sendCommand(SSD1306_SET_PRECHARGE_PERIOD, 0xf1);
    // устанавливаем питание светодиодов VcomH в значение выше чем по умолчанию:
    // 0x30 это увеличит яркость дисплея допустимые значения: 0x00, 0x10, 0x20,
    // 0x30, 0x40, 0x50, 0x60, 0x70
    _sendCommand(SSD1306_SET_VCOM_DESELECT, 0x40);
    // разрешаем отображать содержимое RAM памяти
    _sendCommand(SSD1306_RAM_ON);
    // отключаем инверсию
    _sendCommand(SSD1306_INVERT_OFF);
    // включаем дисплей
    _sendCommand(SSD1306_DISPLAY_ON);
    // чистим экран
    clearDisplay();
}

// обновление только измененных столбцов
void TroykaOLED::update() {
    if (changed.right > -1 || changed.left < _width + 2) {
        changed.left = changed.left < 0 ? 0 : changed.left;
        changed.right = changed.right > _width - 1 ? _width - 1 : changed.right;
        _sendColumns(changed.left, changed.right);
        changed.left = _width + 2;
        changed.right = -1;
    }
}

// обновление всех столбцов
void TroykaOLED::updateAll() {
    _sendColumns(0, _width - 1);
    changed.left = _width + 2;
    changed.right = -1;
}

void TroykaOLED::autoUpdate(bool stateAutoUpdate) {
    _stateAutoUpdate = stateAutoUpdate;
}

void TroykaOLED::setBrightness(uint8_t brightness) {
    _sendCommand(SSD1306_SET_CONTRAST, brightness);
}

void TroykaOLED::clearDisplay() {
    memset(_screenBuffer.asBytes, 0, _width * _height / 8);
    _change(0, _width - 1);
    _AUTO_UPDATE();
}

void TroykaOLED::invertDisplay(bool stateInvert) {
    if (stateInvert) {
        _stateInvert = true;
        _sendCommand(SSD1306_INVERT_ON);
    } else {
        _stateInvert = false;
        _sendCommand(SSD1306_INVERT_OFF);
    }
}

void TroykaOLED::invertText(bool stateInvertText) { _font.invert = stateInvertText; }

void TroykaOLED::textColor(uint8_t color) { _font.color = color; }

void TroykaOLED::imageColor(uint8_t color) { _imageColor = color; }

void TroykaOLED::setFont(const uint8_t* fontData) {
    // сохраняем указатель на первый байт массива в области памяти программ
    _font.fontData = fontData;
    //  сохраняем ширину символов выбранного шрифта читая её из 0 байта массива по указателю fontData
    _font.width = pgm_read_byte(&fontData[0]);
    // сохраняем высоту символов выбранного шрифта читая её из 1 байта массива по указателю fontData
    _font.height = pgm_read_byte(&fontData[1]);
    // сохраняем код первого симола выбран. шрифта читая его из 2 байта массива по указателю fontData
    _font.firstSymbol = pgm_read_byte(&fontData[2]);
    // сохраняем количество символов в выбр шрифте читая их  из 3 байта массива по указателю fontData
    _font.sumSymbol = pgm_read_byte(&fontData[3]);
    //  устанавливаем флаг выбора шрифта
    _font.setFont = true;
    // определяем позицию бита указывающего количество пустых интервалов в массиве шрифта.
    uint16_t i = (uint16_t)_font.sumSymbol * _font.width * _font.height / 8 + 0x04;
    // определяем количество пустых интервалов в массиве шрифта.
    uint16_t j = pgm_read_byte(&fontData[i]);
    // указываем что первый пустой интервал в массиве шрифта находится после символа с кодом (0xFF) и состоит из 0 символов
    _font.startSpace[0] = 0xFF;
    _font.sumSpace[0] = 0;
    // указываем что второй пустой интервал в массиве шрифта находится после символа с кодом (0xFF) и состоит из 0 символов
    _font.startSpace[1] = 0xFF;
    _font.sumSpace[1] = 0;
    // указываем что третий пустой интервал в массиве шрифта находится после символа с кодом (0xFF) и состоит из 0 символов
    _font.startSpace[2] = 0xFF;
    _font.sumSpace[2] = 0;
    // если количество пустых интервалов больше 0
    // сохраняем начало первого пустого интервала символов и размер первого пустого интервала символов
    if (j > 0) {
        _font.startSpace[0] = pgm_read_byte(&fontData[i + 1]);
        _font.sumSpace[0] = pgm_read_byte(&fontData[i + 2]);
    }
    // если количество пустых интервалов больше 1
    // сохраняем начало второго пустого интервала символов и размер второго пустого интервала символов
    if (j > 1) {
        _font.startSpace[1] = pgm_read_byte(&fontData[i + 3]);
        _font.sumSpace[1] = pgm_read_byte(&fontData[i + 4]);
    }
    // если количество пустых интервалов больше 2
    // сохраняем начало третьего пустого интервала символов и размер третьего пустого интервала символов
    if (j > 2) {
        _font.startSpace[2] = pgm_read_byte(&fontData[i + 5]);
        _font.sumSpace[2] = pgm_read_byte(&fontData[i + 6]);
    }
}

void TroykaOLED::setCoding(uint8_t codingName) {
    _codingName = codingName;
}

void TroykaOLED::setCursor(int numX, int numY) {
    if (numX < _width) {
        _numX = numX;
    }
    if (numY < _height) {
        _numY = numY;
    }
}

void TroykaOLED::print(char* data, int x, int y) {
    _print(_codingCP866(data), x, y);
}

void TroykaOLED::print(char ch, int x, int y) {
    char buff[2] = { ch, 0x00 };
    _print(_codingCP866(buff), x, y);
}

void TroykaOLED::print(String str, int x, int y) {
    char data[str.length() + 1];
    str.toCharArray(data, str.length() + 1);
    _print(_codingCP866(data), x, y);
}

void TroykaOLED::print(const char* str, int x, int y) {
    char data[strlen(str) + 1];
    for (uint8_t i = 0; i <= strlen(str); i++) {
        data[i] = str[i];
    }
    _print(_codingCP866(data), x, y);
}

void TroykaOLED::print(int8_t num, int x, int y, uint8_t base) {
    print(int32_t(num), x, y, base);
}

void TroykaOLED::print(uint8_t num, int x, int y, uint8_t base) {
    print(uint32_t(num), x, y, base);
}

void TroykaOLED::print(int16_t num, int x, int y, uint8_t base) {
    print(int32_t(num), x, y, base);
}

void TroykaOLED::print(uint16_t num, int x, int y, uint8_t base) {
    print(uint32_t(num), x, y, base);
}

void TroykaOLED::print(int32_t num, int x, int y, uint8_t base) {
    // определяем количество разрядов числа
    // i = количество разрядов + 2, j = множитель кратный основанию системы счисления
    int8_t i = 2;
    int32_t j = 1;
    while (num / j) {
        j *= base;
        i++;
    }
    // создаём строку k из i символов и добавляем символ(ы) конца строки
    char k[i];
    i--;
    k[i] = 0;
    i--;
    if (num > 0) {
        k[i] = 0;
        i--;
    }
    // создаём строку k из i символов и добавляем символ(ы) конца строки
    uint32_t n = num < 0 ? num * -1 : num;
    while (i) {
        k[i] = _itoa(n % base);
        n /= base;
        i--;
    }
    // заполняем строку k
    if (num >= 0) {
        k[i] = _itoa(n % base);
    } else {
        k[i] = '-';
    }
    //	добавляем первый символ (либо первая цифра, либо знак минус)
    //  выводим строку k
    print(k, x, y);
}

void TroykaOLED::print(uint32_t num, int x, int y, uint8_t base) {
    // определяем количество разрядов числа
    // i = количество разрядов + 1, j = множитель кратный основанию системы счисления
    int8_t i = 1;
    uint32_t j = 1;
    while (num / j) {
        j *= base;
        i++;
    }
    if (num == 0) {
        i++;
    }
    // определяем строку k из i символов и заполняем её
    char k[i];
    i--;
    k[i] = 0;

    while (i) {
        k[i - 1] = _itoa(num % base);
        num /= base;
        i--;
    }
    // выводим строку k
    print(k, x, y);
}

void TroykaOLED::print(double num, int x, int y, uint8_t sum) {
    uint32_t i = 1, j = 0, k = 0;
    j = sum;
    while (j) {
        i *= 10;
        j--;
    }
    // выводим целую часть числа
    print(int32_t(num), x, y);
    // если требуется вывести хоть один знак после запятой, то ...
    if (sum) {
        //  выводим символ разделителя
        print(".");
        // получаем целое число, которое требуется вывести после запятой
        j = num * i * (num < 0 ? -1 : 1);
        j %= i;
        k = j;
        // если полученное целое число равно нулю, то выводим sum раз символ «0»
        if (j == 0) {
            while (sum) {
                print("0");
                sum--;
            }
        } else {
            // иначе, если в полученном целом числе меньше разрядов чем требуется
            // заполняем эти разряды выводя символ «0», после чего выводим само число
            while (j * 10 < i) {
                print("0");
                j *= 10;
            }
            print(k);
        }
    }
}

void TroykaOLED::drawPixel(int16_t x, int16_t y, uint8_t color) {
    _drawPixel(x, y, color);
    _AUTO_UPDATE();
    _last.x = x;
    _last.y = y;
}

void TroykaOLED::drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color) {
    _drawLine(x1, y1, x2, y2, color);
    _AUTO_UPDATE();
    _last.x = x2;
    _last.y = y2;
}

void TroykaOLED::drawLine(int16_t x2, int16_t y2, uint8_t color) {
    drawLine(_last.x, _last.y, x2, y2, color);
}

void TroykaOLED::drawRect(int16_t x1, int16_t y1, int16_t x2, int16_t y2, bool fill,
    uint8_t color) {
    uint64_t col = 0;
    uint64_t frm = 0;
    uint8_t xStart, xEnd, yStart = 0, yEnd = 0;
    // проверяем направление рисования
    if (x2 > x1) {
        xStart = x1;
        xEnd = x2;
    } else {
        xStart = x2;
        xEnd = x1;
    }
    if (y2 > y1) {
        yStart = y1;
        yEnd = y2;
    } else {
        xStart = y2;
        xEnd = y1;
    }
    // рисуем вертикальную палочку и "2 точки" для горизонтальных линий
    for (int16_t j = 0; j < (yEnd - yStart); j++) {
        frm <<= 1;
        if (j == 0 || j == (yEnd - yStart - 1))
            frm |= 1;
        col <<= 1;
        col |= 1;
    }
    // сдвигаем их по оси y
    for (int16_t j = 0; j < yStart; j++) {
        col <<= 1;
        frm <<= 1;
    }

    if (fill) {
        // если сплошной - заливаем столбцы вертикальными палочками
        for (int16_t i = xStart; i < xEnd; i++) {
            if (i >= 0 && i < _width) {
                _stamp(i, 0, col, color);
            }
        }
    } else {
        // если не сплошной - сначала по краям рисуем палочки
        _screenBuffer.column[xStart] |= col;
        _screenBuffer.column[xEnd - 1] |= col;
        // потом остальное заполняем горизонтальными линиями
        for (int16_t i = xStart + 1; i < xEnd - 1; i++) {
            if (i >= 0 && i < _width) {
                _stamp(i, 0, frm, color);
            }
        }
    }
    _AUTO_UPDATE();
}

void TroykaOLED::drawCircle(int16_t x, int16_t y, uint8_t r, bool fill, uint8_t color) {
    // x1,y1 - положительные координаты точек круга с центром 00
    // p - отрицательная парабола
    int16_t x1 = 0, y1 = r, p = 1 - r;
    // цикл будет выполняться пока координата x не станет чуть меньше y
    // прочертит дугу от 0 до 45° - это 1/8 часть круга
    while (x1 < y1 + 1) {
        if (fill) {
            // прорисовываем горизонтальные линии вверху круга (между точками 3 и 1 дуг)
            _drawLine(x - x1, y - y1, x + x1, y - y1, color);
            // прорисовываем горизонтальные линии внизу  круга (между точками 4 и 2 дуг)
            _drawLine(x - x1, y + y1, x + x1, y + y1, color);
            // прорисовываем горизонтальные линии выше середины круга (между точками 7 и 5 дуг)
            _drawLine(x - y1, y - x1, x + y1, y - x1, color);
            // прорисовываем горизонтальные линии выше середины круга (между точками 8 и 6 дуг)
            _drawLine(x - y1, y + x1, x + y1, y + x1, color);
        } else {
            // 1 дуга 0° - 45° (построенная в соответствии с уравнением)
            _drawPixel(x + x1, y - y1, color);
            // 2 дуга 180° - 135° (1 дуга отражённая по вертикали)
            _drawPixel(x + x1, y + y1, color);
            // 3 дуга 360° - 315° (1 дуга отражённая по горизонтали)
            _drawPixel(x - x1, y - y1, color);
            // 4 дуга 180° - 225° (2 дуга отражённая по горизонтали)
            _drawPixel(x - x1, y + y1, color);
            // 5 дуга  90° - 45°  (2 дуга повёрнутая на -90°)
            _drawPixel(x + y1, y - x1, color);
            // 6 дуга  90° - 135° (1 дуга повёрнутая на +90°)
            _drawPixel(x + y1, y + x1, color);
            // 7 дуга 270° - 315° (1 дуга повёрнутая на -90°)
            _drawPixel(x - y1, y - x1, color);
            // 8 дуга 270° - 225° (2 дуга повёрнутая на +90°)
            _drawPixel(x - y1, y + x1, color);
        }
        // если парабола p вышла в положительный диапазон
        if (p >= 0) {
            // сдвигаем её вниз на y1 * 2 (каждый такой сдвиг провоцирет смещение точки y1 первой дуги вниз)
            y1--;
            p -= y1 * 2;
        }
        // с каждым проходом цикла, смещаем точку x1 первой дуги влево и находим новую координату параболы p
        p++;
        x1++;
        p += x1 * 2;
    }
    _AUTO_UPDATE();
    _last.x = x;
    _last.y = y;
}

void TroykaOLED::drawImage(const uint8_t* image, int16_t x, int16_t y, uint8_t mem) {
    // Формат изображения с которым работает эта функция:
    // 1 байт - ширина изображения в битах
    // 2 байт - высота изображения в битах. Высота должна быть кратна 8!
    // последующие байты - слева-направо, сверху-вниз, 8-битные столбцы данных
    int16_t w = getImageWidth(image, mem);
    int16_t h = getImageHeight(image, mem);
    _interpretParameters(x, y, w, h);
    uint64_t col;
    // для столбцов где будет изображение...
    for (int16_t i = 0; i < w; i++) {
        // но только тех из них которые находятся в пределах экрана
        if (_last.x + i < _width && _last.x + i >= 0) {
            // создаем временный пустой столбец
            col = 0;
            // комбинируем 8 битные страницы изображения во временный столбец
            for (int16_t j = 0; j < (h >> 3); j++) {
                if (mem == IMG_ROM)
                    col |= (uint64_t)(pgm_read_byte(&image[j * w + i + 2])) << (8 * j);
                else
                    col |= (uint64_t)image[j * w + i + 2] << (8 * j);
            }
            // в зависимости от "цвета" впечатываем временный столбец на реальный
            _stamp(_last.x + i, _last.y, col, _imageColor);
        }
    }
    _last.x += w;
    _change(_last.x, _last.x + w);
    _AUTO_UPDATE();
}

uint8_t TroykaOLED::getPixel(int16_t x, int16_t y) {
    if (x >= 0 && x < _width && y >= 0 && y < _height)
        return (_screenBuffer.column[x] >> y) == 1;
    else
        // за пределами экрана все черное
        return BLACK;
}

// возвращает ширину изображения в точках, mem - тип памяти
uint8_t TroykaOLED::getImageWidth(const uint8_t* image, uint8_t mem) {
    return (mem == IMG_RAM) ? image[0] : pgm_read_byte(&image[0]);
}

// возвращает высоту изображения в в точках, mem - тип памяти
uint8_t TroykaOLED::getImageHeight(const uint8_t* image, uint8_t mem) {
    return (mem == IMG_RAM) ? image[1] : pgm_read_byte(&image[1]);
}

void TroykaOLED::_interpretParameters(int16_t x, int16_t y, int16_t w, int16_t h) {
    // колонка с которой требуется начать вывод текста ...
    switch (x) {
    // определяем начальную колонку для выравнивания по левому краю.
    case OLED_LEFT:
        _last.x = 0;
        break;
    // определяем начальную колонку для выравнивания по центру
    case OLED_CENTER:
        _last.x = (_width - w) / 2;
        break;
    // определяем начальную колонку для выравнивания по правому краю
    case OLED_RIGHT:
        _last.x = _width - w;
        break;
    // начальной колонкой останется та, на которой был закончен вывод предыдущего
    // текста или изображения
    case OLED_THIS: /*_last.x = _last.x;*/
        break;
    // начальная колонка определена пользователем
    default:
        _last.x = x;
        break;
    }
    // строка с которой требуется начать вывод текста ...
    switch (y) {
    // определяем начальную строку для выравнивания по верхнему краю
    case OLED_TOP:
        _last.y = 0;
        break;
    // определяем начальную строку для выравнивания по центру
    case OLED_CENTER:
        _last.y = (_height - h) / 2;
        break;
    // определяем начальную строку для выравнивания по нижнему краю
    case OLED_BOTTOM:
        _last.y = _height - h;
        break;
    // начальной строкой останется та, на которой выведен предыдущий текст или
    // изображение
    case OLED_THIS: /*_last.y = _last.y;*/
        break;
    // начальная строка определена пользователем
    default:
        _last.y = y;
        break;
    }
}

void TroykaOLED::_print(char* data, int x, int y) {
    // если шрифт не выбран или его высота не кратна 8 пикселям, то выходим из функции
    if (_font.setFont == false || _font.height % 8 > 0) {
        return;
    }
    // определяем количество колонок которое занимают выводимые символы
    uint16_t len = strlen(data) * _font.width;
    if (len > _width) {
        len = _width / _font.width * _font.width;
    }
    // объявляем переменную для хранения номера байта в массиве шрифта
    uint16_t num;
    // объявляем переменные для хранения координат точек
    int x1, y1;
    // объявляем переменную для хранения цвета точек
    bool c;
    // колонка с которой требуется начать вывод текста ...
    switch (x) {
    // определяем начальную колонку для выравнивания по левому краю.
    case OLED_LEFT:
        _numX = 0;
        break;
    // определяем начальную колонку для выравнивания по центру
    case OLED_CENTER:
        _numX = (_width - len) / 2;
        break;
    // определяем начальную колонку для выравнивания по правому краю
    case OLED_RIGHT:
        _numX = _width - len;
        break;
    // начальной колонкой останется та, на которой был закончен вывод предыдущего текста или изображения
    case OLED_THIS:
        _numX = _numX;
        break;
    // начальная колонка определена пользователем
    default:
        _numX = x;
        break;
    }
    // строка с которой требуется начать вывод текста ...
    switch (y) {
    // определяем начальную строку для выравнивания по верхнему краю
    case OLED_TOP:
        _numY = _font.height - 1;
        break;
    // определяем начальную строку для выравнивания по центру
    case OLED_CENTER:
        _numY = (_height - _font.height) / 2 + _font.height;
        break;
    // определяем начальную строку для выравнивания по нижнему краю
    case OLED_BOTTOM:
        _numY = _height;
        break;
    // начальной строкой останется та, на которой выведен предыдущий текст или изображение
    case OLED_THIS:
        _numY = _numY;
        break;
    // начальная строка определена пользователем
    default:
        _numY = y;
        break;
    }
    // пересчитываем количество колонок которое занимают выводимые символы, с учётом начальной позиции
    if (_numX + len > _width) {
        len = (_width - _numX) / _font.width * _font.width;
    }
    // проходим по страницам символов...
    for (int8_t p = 0; p < _font.height / 8; p++) {
        // проходим по выводимым символам...
        for (uint8_t n = 0; n < (len / _font.width); n++) {
            // присваиваем переменной num код выводимого символа
            num = uint8_t(data[n]);
            // если в массиве символов, до кода текущего символа, имеется пустой интервал
            // уменьшаем код текущего символа на количество символов в пустом интервале
            if (_font.startSpace[0] < num) {
                num -= _font.sumSpace[0];
            }
            // если в массиве символов, до кода текущего символа, имеется пустой интервал
            // уменьшаем код текущего символа на количество символов в пустом интервале
            if (_font.startSpace[1] < num) {
                num -= _font.sumSpace[1];
            }
            // если в массиве символов, до кода текущего символа, имеется пустой интервал
            // то уменьшаем код текущего символа на количество символов в пустом интервале
            if (_font.startSpace[2] < num) {
                num -= _font.sumSpace[2];
            }
            // вычитаем код первого символа (с которого начинается массив шрифта)
            num -= _font.firstSymbol;
            // умножаем полученное значение на ширину символа (количество колонок)
            num *= _font.width;
            // умножаем полученное значение на высоту символа (количество страниц)
            num *= _font.height / 8;
            // добавляем количество колонок данного символа, которые уже были выведены на предыдущих страницах
            num += p * _font.width;
            // добавляем количество байт в начале массива шрифта, которые не являются байтами символов
            num += 0x04;
            // проходим по байтам очередного символа
            for (uint8_t k = 0; k < _font.width; k++) {
                // проходим по байтам очередного символа
                for (uint8_t b = 0; b < 8; b++) {
                    // начальная колонка всего текста + (количество выведенных символов * ширина символов) + номер байта текущего символа
                    x1 = _numX + n * _font.width + k;
                    // нижняя строка текста - высота симолов + количество уже выведенных страниц + номер бита байта текущего символа + 1
                    y1 = _numY + p * 8 + b;
                    // цвет точки символа: 1-белый, 0-чёрный
                    c = bitRead(pgm_read_byte(&_font.fontData[num + k]), b);
                    // если цвет текста требуется инвертировать
                    if (_font.invert) {
                        // если установлен фон текста или точка стоит на букве (а не на фоне)
                        if (_font.background || c) {
                            // выводим инвертированную точку
                            _drawPixel(x1, y1, !c);
                        }
                    } else {
                        // если цвет текста не требуется инвертировать
                        // если установлен фон текста или точка стоит на букве (а не на фоне)
                        if (_font.background || c) {
                            // выводим не инвертированную точку
                            _drawPixel(x1, y1, c);
                        }
                    }
                }
            }
        }
    }
    if (_stateAutoUpdate) {
        _sendBuffer();
    }
    // сохраняем координату окончания текста.
    _numX += len;
}

//  параметр: одна цифра от 0 до 15
//  преобразуем цифры 0-9 в символ с кодом 48-57, а цифры 10-15 в символ с кодом 65-71
char TroykaOLED::_itoa(uint8_t num) {
    return char(num + (num < 10 ? 48 : 55));
}

char* TroykaOLED::_codingCP866(char* StrIn) {
    // определяем строку для вывода результата
    char* StrOut = StrIn;
    // переменненые для хранения номера сивола в строках StrIn и StrOut
    uint8_t numIn = 0, numOut = 0;
    // переменненые для хранения текущего кода символа в строках StrIn и StrOut
    uint8_t charThis = StrIn[0], charNext = StrIn[1];
    switch (_codingName) {
    // преобразуем текст из кодировки UTF-8:
    case TXT_UTF8:
        while (charThis > 0 && numIn < 0xFF) {
            // если код текущего символа равен 208, а за ним следует символ с кодом 144...191
            // значит это буква «А»...«п» требующая преобразования к коду 128...175
            if (charThis == 0xD0 && charNext >= 0x90 && charNext <= 0xBF) {
                StrOut[numOut] = charNext - 0x10;
                numIn++;
            } else if (charThis == 0xD0 && charNext == 0x81) {
                // если код текущего символа равен 208, а за ним следует символ с кодом 129
                // значит это буква «Ё» требующая преобразования к коду 240
                StrOut[numOut] = 0xF0;
                numIn++;
            } else if (charThis == 0xD1 && charNext >= 0x80 && charNext <= 0x8F) {
                // если код текущего символа равен 209, а за ним следует символ с кодом 128...143
                // значит это буква «р»...«я» требующая преобразования к коду 224...239
                StrOut[numOut] = charNext + 0x60;
                numIn++;
            } else if (charThis == 0xD1 && charNext == 0x91) {
                // если код текущего символа равен 209, а за ним следует символ с кодом 145
                // значит это буква «ё» требующая преобразования к коду 241
                StrOut[numOut] = 0xF1;
                numIn++;
            } else {
                // иначе не меняем символ
                StrOut[numOut] = charThis;
            }
            // переходим к следующему символу
            numIn++;
            numOut++;
            charThis = StrIn[numIn];
            charNext = StrIn[numIn + 1];
            // добавляем символ конца строки и возвращаем строку StrOut
        }
        StrOut[numOut] = '\0';
        break;
    //преобразуем текст из кодировки WINDOWS-1251:
    case TXT_WIN1251:
        // если код текущего символа строки StrIn больше 0 и номер текушего символа строки StrIn меньше 255
        while (charThis > 0 && numIn < 0xFF) {
            // если код текущего символа равен 192...239
            // значит это буква «А»...«п» требующая преобразования к коду 128...175
            if (charThis >= 0xC0 && charThis <= 0xEF) {
                StrOut[numOut] = charThis - 0x40;
            } else if (charThis >= 0xF0 && charThis <= 0xFF) {
                // если код текущего символа равен 240...255
                // значит это буква «р»...«я» требующая преобразования к коду 224...239
                StrOut[numOut] = charThis - 0x10;
            } else if (charThis == 0xA8) {
                // если код текущего символа равен 168, значит это буква «Ё» требующая преобразования к коду 240
                StrOut[numOut] = 0xF0;
            } else if (charThis == 0xB8) {
                // если код текущего символа равен 184, значит это буква «ё» требующая преобразования к коду 241
                StrOut[numOut] = 0xF1;
            } else {
                // иначе не меняем символ
                StrOut[numOut] = charThis;
            }
            // переходим к следующему символу
            numIn++;
            numOut++;
            charThis = StrIn[numIn];
            // добавляем символ конца строки
        }
        StrOut[numOut] = '\0';
        break;
    }
    // возвращаем строку StrOut
    return StrOut;
}

void TroykaOLED::_stamp(int16_t x, int16_t y, uint64_t body, uint8_t color) {
    if (x >= 0 && x < _width && y >= 0 && y < _height) {
        switch (color) {
        case WHITE:
            _screenBuffer.column[x] |= ((y == 0) ? body : ((y > 0) ? (body << y) : (body >> (-y))));
            break;
        case BLACK:
            _screenBuffer.column[x] &= ~(((y == 0) ? body : ((y > 0) ? (body << y) : (body >> (-y)))));
            break;
        case INVERSE:
            _screenBuffer.column[x] ^= ((y == 0) ? body : ((y > 0) ? (body << y) : (body >> (-y))));
            break;
        default:
            break;
        }
    }
}

void TroykaOLED::_drawPixel(int16_t x, int16_t y, uint8_t color) {
    _stamp(x, y, 1, color);
    _change(x - 1, x + 1);
}

void TroykaOLED::_drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color) {
    int16_t dx = x2 - x1;
    int16_t dy = y2 - y1;
    // рисуем линию по линейному уровнению (y-y1)/(y2-y1) = (x-x1)/(x2-x1)
    // определяем где больше расстояние (по оси x или y)
    // по той оси проходим в цикле, для поиска точек на другой оси
    if (abs(dx) > abs(dy)) {
        if (x1 < x2) {
            for (int16_t x = x1; x <= x2; x++) {
                _drawPixel(x, ((x - x1) * dy / dx + y1), color);
            }
        } else {
            for (int16_t x = x1; x >= x2; x--) {
                _drawPixel(x, ((x - x1) * dy / dx + y1), color);
            }
        }
    } else {
        if (y1 < y2) {
            for (int16_t y = y1; y <= y2; y++) {
                _drawPixel(((y - y1) * dx / dy + x1), y, color);
            }
        } else {
            for (int16_t y = y1; y >= y2; y--) {
                _drawPixel(((y - y1) * dx / dy + x1), y, color);
            }
        }
    }
}

// отправка байта команды
void TroykaOLED::_sendCommand(uint8_t command) {
    _wire->beginTransmission(_i2cAddress);
    _wire->write(0x80);
    _wire->write(command);
    _wire->endTransmission();
}

// то же для команды с параметром
void TroykaOLED::_sendCommand(uint8_t command, uint8_t value) {
    _sendCommand(command);
    _sendCommand(value);
}

// то же для команды с двумя параметрами
void TroykaOLED::_sendCommand(uint8_t command, uint8_t value1, uint8_t value2) {
    _sendCommand(command);
    _sendCommand(value1);
    _sendCommand(value2);
}

// контролируем в пределах каких столбцов происходили изменения чтобы не перерисовывать все
void TroykaOLED::_change(int16_t left, int16_t right) {
    if (left < changed.left) {
        changed.left = left;
    }
    if (right > changed.right) {
        changed.right = right;
    }
}

// отправка буфера (массива _bufferDisplay) в дисплей
void TroykaOLED::_sendBuffer() {
    _sendCommand(SSD1306_ADDR_PAGE, 0, _height / 8 - 1);
    _sendCommand(SSD1306_ADDR_COLUMN, 0, _width - 1);

    for (int16_t i = 0; i < _width * _height / 8; /* realy blank */) {
        _wire->beginTransmission(_i2cAddress);
        _wire->write(0x40);
        for (uint8_t x = 0; x < 16; x++) {
            _wire->write(_screenBuffer.asBytes[i++]);
        }
        _wire->endTransmission();
    }
}

// отправка части буфера (столбцы от start до end) в дисплей
void TroykaOLED::_sendColumns(uint8_t start, uint8_t end) {
    _sendCommand(SSD1306_ADDR_PAGE, 0, _height / 8 - 1);
    _sendCommand(SSD1306_ADDR_COLUMN, start, end);

    for (int16_t i = start * 8; i < (end + 1) * 8; /* realy blank */) {
        _wire->beginTransmission(_i2cAddress);
        _wire->write(0x40);
        for (uint8_t x = 0; x < 8; x++) {
            _wire->write(_screenBuffer.asBytes[i++]);
        }
        _wire->endTransmission();
    }
}
