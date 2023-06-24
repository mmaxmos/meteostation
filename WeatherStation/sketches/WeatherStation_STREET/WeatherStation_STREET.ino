#include <Adafruit_BMP085.h>   //библиотека датчика BMP180
#include <AHT10.h>             //библиотека датчика AHT10

Adafruit_BMP085 bmp;           //Создаем объект для BMP180
AHT10Class aht;                //Создаем объект для AHT10

void setup() 
{
  Serial.begin(9600);
  aht.begin(0x38);
  bmp.begin();
}

void loop() {
  String dataPackage, Tstring, Hstring, Pstring;
  Tstring = (int)round(aht.GetTemperature()); //опрос датчиков
  Hstring = (int)round(aht.GetHumidity());
  Pstring = (int)round(bmp.readPressure()*0.007500637554192); 
  while(Tstring.length() != 3){               //формирование пакета данных
    Tstring +='v';
  }
  while(Hstring.length() != 3){
    Hstring +='v';
  }
  dataPackage = Tstring + Hstring + Pstring;
  Serial.println(dataPackage);               //передача пакета главному модулю
  delay(100);
}
