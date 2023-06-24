#include <Adafruit_BMP085.h>          //библиотека датчика BMP180
#include <AHT10.h>                    //библиотека датчика AHT10
#include <LiquidCrystal_I2C.h>        //библиотека LCD1602
#include <iarduino_RTC.h>             //библиотека RTC
#include <SPI.h>                      //библиотеки для SD
#include <SD.h>

Adafruit_BMP085 bmp;                  //Создаём объект для BMP180
AHT10Class aht;                       //Создаём объект для AHT10
LiquidCrystal_I2C lcd(0x3F, 16,2);    //Создаём объект для LCD1602
iarduino_RTC time(RTC_DS1307);        //Создаёи объект для RTC
File myFile;

bool flag1 = false, flag2 = false, flag3 = false;
int n1 = -1, n2 = 0, n3 = 0, t = 0;
String dataPackage, Tstreet, Hstreet, Pstreet, Timed, Timet, Thome, Hhome, Phome;


void setup() 
{
  Serial.begin(9600);
  Serial1.begin(9600);
  time.begin();
  time.settime(0, 27, 17, 5, 6, 23, 2);
  bmp.begin();
  aht.begin(0x38); 
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  lcd.init();        // Инициализация дисплея  
}

void gettingSensorData()
{
  if (Serial1.available())    
    dataPackage = Serial1.readStringUntil('\n');    // получение пакета данных от выносного модуля
  Tstreet = dataPackage.substring(0,3);            // распаковка пакета 
  Hstreet = dataPackage.substring(3,6);
  Pstreet = dataPackage.substring(6,9);  
  while (Tstreet[Tstreet.length()-1] == 'v')
    Tstreet.remove(Tstreet.length()-1);
  while (Hstreet[Hstreet.length()-1] == 'v')
    Hstreet.remove(Hstreet.length()-1);
  
  Thome = (int)round(aht.GetTemperature());        // опрос датчиков внутреннего модуля
  Hhome = (int)round(aht.GetHumidity());
  Phome = (int)round(bmp.readPressure()*0.007500637554192);

  Timed = time.gettime("d/m/Y"); //получение даты от RTC
  Timet = time.gettime("H:i:s"); //получение времени от RTC
}
 
void saveData(String tc)
{
  int a;
  a = tc.toInt();
  if (abs(a - t) >= 5){
    t = a;
    if (SD.begin(10)){
    myFile = SD.open("weather.txt", FILE_WRITE);
    if (myFile){
      myFile.println();
      myFile.println(Timed);
      myFile.println(Timet);
      myFile.println(Tstreet + 'v' + Hstreet + 'v' + Pstreet);
      myFile.println(Thome + 'v' + Hhome + 'v' + Phome);
      myFile.close();
    }
    }
  }
}

void lcdPrint(String s1, String s2) // вывод данных на экран
{
  lcd.setCursor(0, 0);                 
  lcd.print(s1);
  lcd.setCursor(0, 1);         
  lcd.print(s2);
}

void startPage(String h)
{
  if (h > "04" && h <= "11")
    lcdPrint(" Good morning!! ", "                ");
  else if (h > "11" && h <= "18")
        lcdPrint("   Good         ", "      afternoon! ");
  else if (h > "18" && h <= "23")
        lcdPrint(" Good evening!! ", "                ");
  else if (h > "23" && h <= "04")
        lcdPrint("  Good night!!  ", "                ");  
}

void buttonAnalysis()
{
  bool btnState1 = !digitalRead(3);
  bool btnState2 = !digitalRead(4);
  bool btnState3 = !digitalRead(5);  
  String h;

  if (btnState1 && !flag1) {  //обработчик касания кнопки 1
    flag1 = true;
    n1 *= (-1);
    if (n1 > 0)
    {
        lcd.noDisplay();
        lcd.noBacklight();
        n2 = 0;
   }    
    else
    {
        lcd.backlight();
        lcd.display();
        h = time.gettime("H");
        startPage(h);
        n2 = 0; 
        n3 = 0;
    }
  }
  if (!btnState1 && flag1)    // обработчик отпускания кнопки 1
    flag1 = false; 
  
  if (btnState2 && !flag2) {  // обработчик касания кнопки 2
    flag2 = true;
    n3 = 0;
    n2++;
    if (n2 > 2) n2 = 1;
  }
  if (!btnState2 && flag2)    // обработчик отпускания кнопки 2
    flag2 = false;  

  if (btnState3 && !flag3) {  // обработчик касания кнопки 3
    flag3 = true;
    n3++;
    if (n3 > 4) n3 = 0;
  }
  if (!btnState3 && flag3)   // обработчик отпускания кнопки 3
    flag3 = false;  
}

void choiceDataForPrint(int n3, String s1, String s2, String s3, String s4, String s5, String s6) // обработка режима кнопки 3 и вывод показаний 
{
   switch(n3){
      case 0:
        lcdPrint(s1, "     data    -->");
        break;
      case 1:
        lcdPrint("  Temperature:    ", "     T=" + s2 + char(223) + "C      ");
        break;
      case 2:
        lcdPrint("   Humidity:    ", "     H=" + s3 + "%       ");
        break;
      case 3:
        lcdPrint("   Pressure:    ", "   P=" + s4 + "mmhg      ");
        break;
      case 4:
        lcdPrint("Date: " + s5, "Time: " + s6);
        break;  
      break;
    }  
}

void loop() {
  String tc;
  gettingSensorData();      // получение данных от датчиков
  buttonAnalysis();         // обработка нажатий кнопок
  tc=time.gettime("i");
  saveData(tc);               //сохранение показаний датчиков
  switch (n2) {             // обработка режимов кнопки 2
    case 1:                 // выбор режима 1 кнопки 2 
      choiceDataForPrint(n3, "   House block      ", Thome, Hhome, Phome, Timed, Timet);        // вывод на экран информации c датчиков внутреннего модуля
      break;
    case 2:                 // выбор режима 2 кнопки 2
      choiceDataForPrint(n3, "  Street block     ", Tstreet, Hstreet, Pstreet, Timed, Timet); // вывод на экран информации с датчиков выносного модуля
      break;
    break;
  }
}
