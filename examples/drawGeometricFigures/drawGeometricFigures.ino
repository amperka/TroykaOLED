// библиотека для работы с OLED-дисплеем
#include <TroykaOLED.h>
// создаём объект для работы с дисплеем
// и передаём I²C адрес дисплея
TroykaOLED myOLED(0x3C);

void setup() {
  // инициализируем дисплей
  myOLED.begin();
  // получаем ширину и высоту дисплея
  int width = myOLED.getWidth();
  int height = myOLED.getHeigth();
  // рисуем две диагонали
  myOLED.drawLine(0, 0, width - 1, height - 1);
  myOLED.drawLine(0, height - 1, width - 1, 0);
  // рисуем окружность в центре дисплея и радиусом 30
  myOLED.drawCircle(width / 2, height / 2, 30);
  // рисуем диск (закрашенную окружность) в центре дисплея и радиусом 20
  myOLED.drawCircle(width / 2, height / 2, 20, true);
}

void loop() {

}


