// библиотека для работы с OLED-дисплеем
#include <TroykaOLED.h>
// создаём объект для работы с дисплеем
// и передаём I²C адрес дисплея
TroykaOLED myOLED(0x3C);

void setup() {
  // инициализируем дисплей
  myOLED.begin();
  // выбираем кодировку символов: CP866, TXT_UTF8 или WIN1251
  myOLED.setCoding(TXT_UTF8);
  //myOLED.setCoding(TXT_CP866);
  //myOLED.setCoding(TXT_WIN1251);
  // выбираем шрифт 6×8 с поддержкой кириллицы
  myOLED.setFont(fontRus6x8);
  // печатаем строку с координатами начала текста
  myOLED.print("Привет мир!", OLED_CENTER, 10);
  // выбираем шрифт 12×10 с поддержкой кириллицы
  myOLED.setFont(fontRus12x10);
  // печатаем строку с координатами начала текста
  myOLED.print("Амперка", OLED_CENTER, 20);
}

void loop() {

}
