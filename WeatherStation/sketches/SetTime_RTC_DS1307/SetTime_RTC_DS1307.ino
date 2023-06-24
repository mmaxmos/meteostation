#include <iarduino_RTC.h> //библиотека RTC DS1307

iarduino_RTC time(RTC_DS1307);

void setup() {
    Serial.begin(9600);
    time.begin();
    time.settime(35,02,17,25,05,23,4);  // 35  сек, 02 мин, 17 час, 25 мая, 2023 года, четверг
}
void loop(){
  if(millis()%1000==0){ // если прошла 2 секунда
      Serial.println(time.gettime("d-m-Y, H:i:s")); // выводим время
      delay(1); // приостанавливаем на 1 мс, чтоб не выводить время несколько раз за 1мс
    }
}
