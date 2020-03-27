// библиотека для работы с OLED-дисплеем
#include <TroykaOLED.h>
// создаём объект для работы с дисплеем
// и передаём I²C адрес дисплея
TroykaOLED myOLED(0x3C);

void setup() {
    // инициализируем дисплей
    myOLED.begin();
    // выбираем кодировку символов: CP866, UTF8 или CP1251
    myOLED.setCoding(Encoding::UTF8);
    //myOLED.setCoding(Encoding::CP866);
    //myOLED.setCoding(Encoding::CP1251);
    // выбираем шрифт 6×8
    myOLED.setFont(alphabet6x8);
    // печатаем строку с координатами начала текста
    myOLED.print("Привет мир!", OLED_CENTER, 10);
    // выбираем шрифт 12×16
    myOLED.setFont(alphabet12x16);
    // печатаем строку с координатами начала текста
    myOLED.print("Амперка", OLED_CENTER, 20);
}

void loop() {
}
