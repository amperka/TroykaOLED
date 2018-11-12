// библиотека для работы с OLED-дисплеем
#include <TroykaOLED.h>
// создаём объект для работы с дисплеем
// и передаём I²C адрес дисплея
TroykaOLED myOLED(0x3C);

void setup() {
  // инициализируем дисплей
  myOLED.begin();
  // выбираем шрифт 8×8
  myOLED.setFont(font6x8);
  // печатаем строку
  myOLED.print("Hello world!", 25, 0);
  // выбираем шрифт 12×10
  myOLED.setFont(font12x10);
  // печатаем строку
  myOLED.print("Amperka", 20, 20);
  // инвертируем последующий текст
  myOLED.invertText("true");
  myOLED.print("OLED", 40, 40);
}

void loop() {

}
