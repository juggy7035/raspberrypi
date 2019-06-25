#include <LiquidCrystal.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include "RTClib.h"
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
RTC_DS1307 rtc;


Adafruit_ADS1015 ads1015;
File myFile;
float tempACT=0; int counterMEASURE=0;
float tempREF=0;
float factor = 0.003;
float ACT;
float REF;
int timer = 0;
int lcd_key     = 0;
int adc_key_in  = 0;
int Screen1 = 0;
int Screen2 = 0;
int choice = 0;
int interval = 100;
int sensitivity = 6;
int recordKey = 0;
int duration = 0;
int startTime = 0;
int currentTime = 0;
int checker = 0;
int totalPoints = 0;
int pinCS = 53;

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

void setup()
{
  SD.begin();
  pinMode(pinCS, OUTPUT);
  myFile = SD.open("ACT.txt", FILE_WRITE);
  ads1015.begin();
  ads1015.setGain(GAIN_TWOTHIRDS); 
  lcd.begin(16, 2);              // start the library
  //-----------------------------------------------
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  //-------------------------------------------------

  
  }
}


// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  
 return btnNONE;  // when all others fail, return this...
}


void gain(int sens){
  switch (sens){
    case 6: lcd.print("6.144"); 
    ads1015.setGain(GAIN_TWOTHIRDS); factor = 0.003;
    break;
    case 5: lcd.print("4.096"); 
    ads1015.setGain(GAIN_ONE); factor = 0.002;
    break;
    case 4: lcd.print("2.048"); 
    ads1015.setGain(GAIN_TWO); factor = 0.001;
    break;
    case 3: lcd.print("1.024"); 
    ads1015.setGain(GAIN_FOUR); factor = 0.0005;
    break;
    case 2: lcd.print("0.512"); 
    ads1015.setGain(GAIN_EIGHT); factor = 0.00025;
    break;
    case 1: lcd.print("0.256"); 
    ads1015.setGain(GAIN_SIXTEEN); factor = 0.000125;
    break;
    
}

}
void Clock(){
  DateTime now = rtc.now();
  lcd.clear();
  lcd.setCursor(4,0);
  if (now.hour() < 10){lcd.print("0"); lcd.print(now.hour());} else{lcd.print(now.hour());}
  lcd.print(":");
  if (now.minute() < 10){lcd.print("0"); lcd.print(now.minute());} else{lcd.print(now.minute());}
  lcd.print(":");
  if (now.second() < 10){lcd.print("0"); lcd.print(now.second());} else{lcd.print(now.second());}
}

void recording(){
  lcd.clear();
  myFile = SD.open("ACT.txt", FILE_WRITE);
  tempACT = 0; tempREF = 0;
  startTime = 0; currentTime = 0; checker = 0; totalPoints = 0;
  startTime = millis();
  lcd.setCursor(0,0);
  lcd.print(startTime);
  currentTime = millis();
  while ((currentTime-startTime < duration*1000)){
      currentTime = millis();
      lcd.setCursor(0,0);
      lcd.print((currentTime-startTime));
      lcd.print(" ");
      lcd.print(duration*1000);
      tempACT += ads1015.readADC_Differential_0_1() * factor;
      tempREF += ads1015.readADC_Differential_2_3() * factor;
      checker += 1;
      
      if (((currentTime-startTime)%interval < 10) and (checker > 10)){
        lcd.setCursor(0,1);
        lcd.print(currentTime-startTime);
        //RECORD tempACT/checker and tempREF/checker

          myFile.println(currentTime-startTime); 
          
          
        tempACT = 0; tempREF = 0; checker = 0;
        totalPoints += 1;
      }
   }   
  lcd.clear();
  myFile.println("  ");
  myFile.close();
  lcd.setCursor(0,1);
  lcd.print(totalPoints);
  delay(3000);
  lcd.clear();
  recordKey = 0; choice = 0;
}

void record(int key2){
  Clock();
  lcd.setCursor(0,1);
  lcd.print("< ");
  lcd.print(interval);
  lcd.print("ms ~ ");
  lcd.print(duration);
  lcd.print("s");
  lcd.setCursor(15,1);
  lcd.print(">");
  if (key2 == 3){choice = 0; recordKey = 0;}
  if (key2 == 0){recording();}
}

void Menu(int key){

if ((choice != 0) and (key == 1)){
  lcd.clear();
  choice--;
}

if ((choice != 4) and (key == 2)){
  lcd.clear();
  choice++;
}

if (choice == 0) {
lcd.setCursor(0,0);
lcd.print("ACT: ");
lcd.print(ACT);
lcd.print("V   ");
lcd.setCursor(0,1);
lcd.print("REF: ");
lcd.print(REF);
lcd.print("V   ");
}

if (choice == 1) {
lcd.setCursor(0,0);
lcd.print("REF: ");
lcd.print(REF);
lcd.print("V   ");
lcd.setCursor(0,1);
lcd.print(">SENS: ");
gain(sensitivity);
lcd.print(" V");
if ((choice == 1) and (key == 0) and (sensitivity != 6)){ sensitivity++;}
if ((choice == 1) and (key == 3) and (sensitivity != 1)){ sensitivity--;}
}

if (choice == 2) {
lcd.setCursor(0,0);
lcd.print("SENS: ");
gain(sensitivity);
lcd.print(" V");
lcd.setCursor(0,1);
lcd.print(">INTRVL: ");
lcd.print(interval);
lcd.print(" ms   ");
if ((choice == 2) and (key == 0)){ interval += 100;}
if ((choice == 2) and (key == 3) and (interval != 100)){ interval-= 100;}
}

if (choice == 3) {
lcd.setCursor(0,0);
lcd.print("INTRVL: ");
lcd.print(interval);
lcd.print(" ms   ");
lcd.setCursor(0,1);
lcd.print(">DUR: ");
lcd.print(duration);
lcd.print(" s  ");
if ((choice == 3) and (key == 0)){ duration += 5;}
if ((choice == 3) and (key == 3) and (duration != 0)){ duration-= 5;}
}

if (choice == 4) {
lcd.setCursor(0,0);
lcd.print("DUR: ");
lcd.print(duration);
lcd.print(" s");
lcd.setCursor(0,1);
lcd.print(">RECORD");
}




}



void loop()
{
 tempACT += ads1015.readADC_Differential_0_1() * factor;
 tempREF += ads1015.readADC_Differential_2_3() * factor;
  counterMEASURE += 1;
 if (counterMEASURE == 5){
  ACT = tempACT/5; REF = tempREF/5;
  tempACT = 0; tempREF = 0;
  counterMEASURE = 0;
 }
 lcd_key = read_LCD_buttons();  // read the buttons
 if ((choice == 4) and (lcd_key == 4)){recordKey = 1;}
 if ((choice == 4) and (recordKey == 1)){ record(lcd_key); timer = duration;}
 else{Menu(lcd_key);}
 delay(100);
}
