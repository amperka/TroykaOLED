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

TroykaOLED::TroykaOLED(uint8_t i2cAddress, uint8_t width, uint8_t height) {
    _i2cAddress = i2cAddress;
    _width = width;
    _height = height;
    _stateInvert = false;
    _stateAutoUpdate = true;
    _imageColor = WHITE;
    _codingName = Encoding::UTF8;
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
    if (_changedColumns.right > -1 || _changedColumns.left < _width + 2) {
        _changedColumns.left = _changedColumns.left < 0 ? 0 : _changedColumns.left;
        _changedColumns.right = _changedColumns.right > _width - 1 ? _width - 1 : _changedColumns.right;
        _sendColumns(_changedColumns.left, _changedColumns.right);
        _changedColumns.left = _width + 2;
        _changedColumns.right = -1;
    }
}

// обновление всех столбцов
void TroykaOLED::updateAll() {
    _sendColumns(0, _width - 1);
    _changedColumns.left = _width + 2;
    _changedColumns.right = -1;
}

void TroykaOLED::autoUpdate(bool stateAutoUpdate) {
    _stateAutoUpdate = stateAutoUpdate;
}

void TroykaOLED::setBrightness(uint8_t brightness) {
    _sendCommand(SSD1306_SET_CONTRAST, brightness);
}

void TroykaOLED::clearDisplay() {
    memset(_screenBuffer.asBytes, 0, _width * _height / 8);
    _markChangedColumns(0, _width - 1);
    if (_stateAutoUpdate) {
        update();
    }
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
    _font.data = fontData;

    _font.width = pgm_read_byte(&fontData[0]);
    _font.height = pgm_read_byte(&fontData[1]);
    char type = pgm_read_byte(&fontData[2]);

    if (_font.height % 8 != 0) {
        _font.setFont = false;
        return;
    }

    if (type == 'D') {
        _font.remap = digitsFontRemap;
        _font.setFont = true;
    } else if (type == 'A') {
        _font.remap = alphabetFontRemap;
        _font.setFont = true;
    } else {
        _font.setFont = false;
    }

    _font.color = WHITE;
}

void TroykaOLED::setCoding(Encoding codingName) { _codingName = codingName; }

void TroykaOLED::setCursor(int16_t x, int16_t y) {
    _last.x = x;
    _last.y = y;
}

void TroykaOLED::print(char character, int16_t x, int16_t y) {
    _print(character, x, y);
}

void TroykaOLED::print(char* line, int16_t x, int16_t y) {
    _print(_encodeToCP866((uint8_t*)line), x, y);
}

void TroykaOLED::print(String str, int16_t x, int16_t y) {
    char data[str.length() + 1];
    str.toCharArray(data, str.length() + 1);
    _print(_encodeToCP866((uint8_t*)data), x, y);
}

void TroykaOLED::print(const char* line, int16_t x, int16_t y) {
    char data[strlen(line) + 1];
    for (uint8_t i = 0; i <= strlen(line); i++) {
        data[i] = line[i];
    }
    _print(_encodeToCP866((uint8_t*)data), x, y);
}

void TroykaOLED::print(int8_t number, int16_t x, int16_t y, uint8_t base) {
    if (base == 10 && number < 0) {
        _radixConverter(-number, true, 8, base, x, y);
    } else {
        _radixConverter(number, false, 8, base, x, y);
    }
}

void TroykaOLED::print(uint8_t number, int16_t x, int16_t y, uint8_t base) {
    _radixConverter(number, false, 8, base, x, y);
}

void TroykaOLED::print(int16_t number, int16_t x, int16_t y, uint8_t base) {
    if (base == 10 && number < 0) {
        _radixConverter(-number, true, 16, base, x, y);
    } else {
        _radixConverter(number, false, 16, base, x, y);
    }
}

void TroykaOLED::print(uint16_t number, int16_t x, int16_t y, uint8_t base) {
    _radixConverter(number, false, 16, base, x, y);
}

void TroykaOLED::print(int32_t number, int16_t x, int16_t y, uint8_t base) {
    if (base == 10 && number < 0) {
        _radixConverter(-number, true, 32, base, x, y);
    } else {
        _radixConverter(number, false, 32, base, x, y);
    }
}

// служебная подпрограмма для конвертора систем счисления
uint8_t TroykaOLED::_punchDigits(uint32_t number, char* digitsArray, uint8_t position, uint8_t base) {
    while (number > 0) {
        digitsArray[position] = _itoa(number % base);
        number /= base;
        position--;
    }
    return position;
}

void TroykaOLED::_radixConverter(uint32_t number, bool sign, uint8_t bits,
    uint8_t base, int16_t x, int16_t y) {
    // убираем лишние "минусы"-FF из исходного числа
    switch (bits) {
    case 8:
        number = number & 0xFF;
        break;
    case 16:
        number = number & 0xFFFF;
        break;
    default:
        break;
    }
    if (number == 0) {
        print("0", x, y);
        return;
    }
    char dig[bits + 1];
    dig[bits] = 0;
    uint8_t pos = bits - 1;
    // печатаем в массив цифры
    pos = _punchDigits(number, dig, pos, base);
    // добавляем минус спереди
    dig[pos] = '-';

    if (sign)
        // если число было отрицательное - выводим вместе с минусом
        print(&dig[pos], x, y);
    else
        // иначе выводим без минуса
        print(&dig[pos + 1], x, y);
}

void TroykaOLED::print(double number, int16_t x, int16_t y, uint8_t sum) {
    bool sign = false;
    // число отрицательное?
    if (number < 0) {
        number = -number;
        sign = true;
    }
    char dig[12 + sum];
    dig[11 + sum] = 0;
    uint32_t integerPart, fractionPart;
    // берем целую часть
    integerPart = (int32_t)number;
    // ищем дробную часть
    double temp = number - integerPart;
    for (int16_t i = 0; i < sum; i++) {
        temp *= 10;
    }
    fractionPart = (uint32_t)temp;

    uint8_t pos = 10 + sum;
    // печатаем в массив дробную часть
    uint8_t pos2 = _punchDigits(fractionPart, dig, pos, 10);
    if(pos2 == pos) {
        dig[pos--] = '0';
    }
    pos = pos2;
    // теперь точку
    dig[pos--] = '.';
    // печатаем в массив целую часть
    pos2 = _punchDigits(integerPart, dig, pos, 10);
    if(pos2 == pos) {
        dig[pos--] = '0';
    }
    pos = pos2;
    // добавляем спереди минус
    dig[pos] = '-';

    if (sign)
        // если число было отрицательное - выводим вместе с минусом
        print(&dig[pos], x, y);
    else
        // иначе выводим без минуса
        print(&dig[pos + 1], x, y);
}

void TroykaOLED::drawPixel(int16_t x, int16_t y, uint8_t color) {
    _drawPixel(x, y, color);
    if (_stateAutoUpdate) {
        update();
    }
    _last.x = x;
    _last.y = y;
}

void TroykaOLED::drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color) {
    _drawLine(x1, y1, x2, y2, color);
    if (_stateAutoUpdate) {
        update();
    }
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
    _markChangedColumns(xStart, xEnd);
    if (_stateAutoUpdate) {
        update();
    }
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
    if (_stateAutoUpdate) {
        update();
    }
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
    _markChangedColumns(_last.x, _last.x + w);
    _last.x += w;
    if (_stateAutoUpdate) {
        update();
    }
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

uint8_t TroykaOLED::_charToGlyph(char c) const {
    return pgm_read_byte(&_font.remap[(uint8_t)c]);
}

void TroykaOLED::_print(char c, int16_t x, int16_t y) {
    uint64_t col;
    uint8_t saveColor = WHITE;

    // если инверсия фонта включена, сначала отрисовываем квадрат основным цветом
    // а потом на него выведем инверсным цветом символ
    if (_font.invert) {
        drawRect(x, y, x + _font.width, y + _font.height, true, _font.color);
        saveColor = _font.color;
        _font.color = (_font.color == WHITE) ? BLACK : WHITE;
    }

    uint8_t offset = _charToGlyph(c);
    // если фонт валидный и символ хотя-бы частично в пределах экрана
    if (_font.setFont == true && x < _width && x + _font.width >= 0
        && y < _height && y + _font.height >= 0) {
        // для столбцов в которых будет отрисован символ
        for (int16_t i = 0; i < _font.width; i++) {
            // но только тех из них которые находятся в пределах экрана
            if (x + i < _width && x + i >= 0) {
                // создаем временный пустой столбец
                col = 0;
                // для каждой 8 битной страницы шрифта
                for (int16_t j = 0; j < (_font.height >> 3); j++) {
                    // отштамповать страницу шрифта на временный столбец
                    col |= (uint64_t)(pgm_read_byte(&_font.data[3 + (offset * (_font.height >> 3) + j) * _font.width + i /*+ 4*/])) << (8 * j);
                }
                // в зависимости от "цвета" впечатываем временный столбец на реальный предварительно сдвинув
                _stamp(x + i, y, col, _font.color);
            }
        }
    }

    if (_font.invert) {
        _font.color = saveColor;
    }
    _markChangedColumns(x, x + _font.width);
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

void TroykaOLED::_print(char* line, int16_t x, int16_t y) {
    int16_t len = strlen(line);
    _interpretParameters(x, y, len * _font.width, _font.height);

    for (int16_t i = 0; i < len; i++) {
        int16_t currentX = _last.x + (i * _font.width);
        if (currentX < _width && currentX > -_font.width) {
            _print(line[i], currentX, _last.y);
        }
    }
    if (_stateAutoUpdate) {
        update();
    }
}

//  параметр: одна цифра от 0 до 15
//  преобразуем цифры 0-9 в символ с кодом 48-57, а цифры 10-15 в символ с кодом 65-71
char TroykaOLED::_itoa(uint8_t number) { return uint8_t(number + (number < 10 ? '0' : 'A')); }

char* TroykaOLED::_encodeToCP866(uint8_t* strIn) {
    // определяем строку для вывода результата
    uint8_t* strOut = strIn;
    // переменненые для хранения номера сивола в строках strIn и strOut
    uint8_t numIn = 0, numOut = 0;
    // переменненые для хранения текущего кода символа в строках strIn и strOut
    uint8_t charThis = strIn[0], charNext = strIn[1];
    switch (_codingName) {
    // преобразуем текст из кодировки UTF-8:
    case Encoding::UTF8:
        while (charThis > 0 && numIn < 0xFF) {
            // если код текущего символа равен 208, а за ним следует символ с кодом
            // 144...191 значит это буква «А»...«п» требующая преобразования к коду
            // 128...175
            if (charThis == 0xD0 && charNext >= 0x90 && charNext <= 0xBF) {
                strOut[numOut] = charNext - 0x10;
                numIn++;
            } else if (charThis == 0xD0 && charNext == 0x81) {
                // если код текущего символа равен 208, а за ним следует символ с кодом
                // 129 значит это буква «Ё» требующая преобразования к коду 240
                strOut[numOut] = 0xF1;
                numIn++;
            } else if (charThis == 0xD1 && charNext >= 0x80 && charNext <= 0x8F) {
                // если код текущего символа равен 209, а за ним следует символ с кодом
                // 128...143 значит это буква «р»...«я» требующая преобразования к коду
                // 224...239
                strOut[numOut] = charNext + 0x61;
                numIn++;
            } else if (charThis == 0xD1 && charNext == 0x91) {
                // если код текущего символа равен 209, а за ним следует символ с кодом
                // 145 значит это буква «ё» требующая преобразования к коду 241
                strOut[numOut] = 0xF2;
                numIn++;
            } else {
                // иначе не меняем символ
                strOut[numOut] = charThis;
            }
            // переходим к следующему символу
            numIn++;
            numOut++;
            charThis = strIn[numIn];
            charNext = strIn[numIn + 1];
            // добавляем символ конца строки и возвращаем строку strOut
        }
        strOut[numOut] = 0;
        break;
    //преобразуем текст из кодировки WINDOWS-1251:
    case Encoding::CP1251:
        // если код текущего символа строки strIn больше 0 и номер текушего символа
        // строки strIn меньше 255
        while (charThis > 0 && numIn < 0xFF) {
            // если код текущего символа равен 192...239
            // значит это буква «А»...«п» требующая преобразования к коду 128...175
            if (charThis >= 0xC0 && charThis <= 0xEF) {
                strOut[numOut] = charThis - 0x40;
            } else if (charThis >= 0xF0) {
                // если код текущего символа равен 240...255
                // значит это буква «р»...«я» требующая преобразования к коду 224...239
                strOut[numOut] = charThis - 0x10;
            } else if (charThis == 0xA8) {
                // если код текущего символа равен 168, значит это буква «Ё» требующая
                // преобразования к коду 240
                strOut[numOut] = 0xF0;
            } else if (charThis == 0xB8) {
                // если код текущего символа равен 184, значит это буква «ё» требующая
                // преобразования к коду 241
                strOut[numOut] = 0xF1;
            } else {
                // иначе не меняем символ
                strOut[numOut] = charThis;
            }
            // переходим к следующему символу
            numIn++;
            numOut++;
            charThis = strIn[numIn];
            // добавляем символ конца строки
        }
        strOut[numOut] = 0;
        break;
    }
    // возвращаем строку strOut
    return (char*)strOut;
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
    _markChangedColumns(x - 1, x + 1);
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
void TroykaOLED::_markChangedColumns(int16_t left, int16_t right) {
    if (left < _changedColumns.left) {
        _changedColumns.left = left;
    }
    if (right > _changedColumns.right) {
        _changedColumns.right = right;
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
