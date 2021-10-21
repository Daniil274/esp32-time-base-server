#include <FS.h>
#include <SPIFFS.h>

#include <AsyncEventSource.h>
#include <AsyncWebSocket.h>
#include <AsyncWebSynchronization.h>
#include <ESPAsyncWebServer.h>
#include <StringArray.h>
#include <WebAuthentication.h>
#include <WebHandlerImpl.h>
#include <WebResponseImpl.h>

#include <LiquidCrystal.h>
#include <Time.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#define i2c_address 0x50

const char* ssid = "ESP-SERVER";
const char* passwd = "2810002606";

AsyncWebServer server(80);

DS1307RTC Clock;

LiquidCrystal lcd(19, 23, 2, 4, 18, 17);

bool work = false;
void printTime(){
  tmElements_t nowTM;
  Clock.read(nowTM);
  lcd.setCursor(11,0);
  lcd.print(addZero(nowTM.Hour));
  lcd.print(":");
  lcd.print(addZero(nowTM.Minute));

}
void printDate(){
  tmElements_t nowTM;
  Clock.read(nowTM);
  lcd.setCursor(8, 1);
  lcd.print(addZero(nowTM.Day));
  lcd.print(".");
  lcd.print(addZero(nowTM.Month));
  lcd.print(".");
  lcd.print(1970+nowTM.Year-2000);
}
void printStatus(){
  lcd.setCursor(0,0);
  if(work==false) lcd.print("Stopped!");
  else{
    lcd.print("Running!");
  }
}
void buttonListen(){
  if(digitalRead(33)){
    work=!work;
    delay(500);
  }
}
String addZero(uint8_t number){
  if(number>=10) return String(number);
  else{
   return "0"+String(number);
  }
}
void setup() {
  //Start serial
  WiFi.softAP(ssid,passwd);
  lcd.begin(16, 2);
  pinMode(33, INPUT);
  digitalWrite(33, LOW);
  server.on("/set-time", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200,"text/html","<script>window.location.href = '/set-time-in?time='+Math.trunc(Date.now()/1000)</script>");
  });
  server.on("/set-time-in", HTTP_GET, [](AsyncWebServerRequest *request){
    String time_s = request->getParam("time")->value();
    time_t time = time_s.toInt()+3600*3;
    Clock.set(time);
    printTime();
  printDate();
    request->send(200,"text/plain","time updated to " + time_s);
  });
  server.begin();
}

void loop() {
  printTime();
  printDate();
  printStatus();
  buttonListen();
}
