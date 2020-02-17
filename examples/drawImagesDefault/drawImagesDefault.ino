// библиотека для работы с OLED-дисплеем
#include <TroykaOLED.h>
// создаём объект для работы с дисплеем
// и передаём I²C адрес дисплея
TroykaOLED myOLED(0x3C);

void setup() {
    // инициализируем дисплей
    myOLED.begin();
    // выводим изображение сигнала связи
    myOLED.drawImage(signal4, OLED_LEFT, OLED_TOP);
    // выводим изображение антенны
    myOLED.drawImage(antenna, 12, OLED_TOP);
    // выводим изображение bluetooth
    myOLED.drawImage(bluetooth, 24, OLED_TOP);
    // выводим изображение батарейки
    myOLED.drawImage(battery3, OLED_RIGHT, OLED_TOP);
    // выводим изображение письма
    myOLED.drawImage(message, OLED_CENTER, OLED_CENTER);
    myOLED.update();
}

void loop() {
}
