#include<LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#include <pt.h>

#define PT_DELAY(pt, ms, ts) \
  ts = millis(); \
  PT_WAIT_WHILE(pt, millis()-ts < (ms));

#define PIN_LEDB 11 //น้ำเงืน
#define PIN_LEDR 12 //ดำ
#define PIN_SC A0
#define PIN_GC A1
#define but1 2      //ส้ม
#define but2 3      //น้ำตาล
#define but3 A2     //ขาว
#define BUZZ 10

#define Standby 11
#define Alert 12
#define PARK 2

String temp = "temp";
String val = "Standby";
String sendData = "NaN";

int Status = Standby;

struct pt pt_taskLED;
struct pt pt_taskSendSerial;

///////////////////////////////////////////////////////
PT_THREAD(taskSendSerial(struct pt* pt))
{
  static uint32_t ts;

  PT_BEGIN(pt);

  while (1)
  {
    sendSerial();
    PT_DELAY(pt, 600, ts);
  }

  PT_END(pt);
}


///////////////////////////////////////////////////////
void soundCheck()
{
  int sensorValue = analogRead(PIN_SC);
  if (sensorValue > 900)
  {
    lcd.clear();
    lcd.print("EMERGENCY !!!");
    digitalWrite(Status, LOW);
    Status = Alert;
    sendData = "S";
    taskSendSerial(&pt_taskSendSerial);
    //send Too Lound
  }
}

void gasCheck()
{
  int sensorValue = analogRead(PIN_GC);
  if (sensorValue > 700)
  {
    lcd.clear();
    lcd.print("Gas Dectect");
    digitalWrite(Status, LOW);
    Status = Alert;
    digitalWrite(Status, HIGH);
    analogWrite(BUZZ, 100);
    delay(500);
    analogWrite(BUZZ, 0);
    delay(500);
    analogWrite(BUZZ, 100);
    delay(500);
    analogWrite(BUZZ, 0);
    delay(500);
    analogWrite(BUZZ, 100);
    delay(500);
    analogWrite(BUZZ, 0);
    delay(500);
    analogWrite(BUZZ, 100);
    delay(500);
    analogWrite(BUZZ, 0);
    sendData = "G";
    taskSendSerial(&pt_taskSendSerial);
    //send
  }
}

void buttonPress()
{
  int pres = 0;
  //Serial.print(String(digitalRead(but1)));
  while (!digitalRead(but1))
  {
    //Serial.println("Press1");
    digitalWrite(Status, LOW);
    Status = Alert;
    pres = 1;
  }
  while (digitalRead(but2))
  {
    //Serial.println("Press2");
    digitalWrite(Status, LOW);
    Status = Alert;
    pres = 2;
  }
  while(digitalRead(but3))
  {
    //Serial.println("Press3");
    digitalWrite(Status, LOW);
    Status=Alert;
    pres=3;
  }
  if(pres==1)
  {
      lcd.clear();
      lcd.print("Calling...");
      //Serial.print("Press1");
      sendData="1";
      taskSendSerial(&pt_taskSendSerial);
      //send
  }
  if(pres==2)
  {
      lcd.clear();
      lcd.print("Calling...");
      //Serial.print("Press2");
      sendData="2";
      taskSendSerial(&pt_taskSendSerial);
      //send
  }
  if(pres==3)
  {
      lcd.clear();
      lcd.print("  EMERGENCY !!");
      //Serial.print("Press3");
      sendData="3";
      taskSendSerial(&pt_taskSendSerial);
      analogWrite(BUZZ,100);
      delay(300);
      analogWrite(BUZZ,0);
      delay(300);
      analogWrite(BUZZ,100);
      delay(300);
      analogWrite(BUZZ,0);
      delay(300);
      analogWrite(BUZZ,100);
      delay(300);
      analogWrite(BUZZ,0);
      //send
  }
  }

  ///////////////////////////////////////////////////////
  void recieveSerial() {
  if (Serial1.available() > 0) {
    val = Serial1.readStringUntil('\r');
    //Serial.print(" Recieve : ");
    //Serial.println(val);
    if(val!=temp)
    {
      Serial.print("Recieve : ");
      Serial.println(val);
      if(val=="/clear")
      {
        digitalWrite(Status, LOW);
        lcd.clear();
        lcd.print("Standby");
        Status=Standby;
        sendData="";
      }
      else
      {
        lcd.clear();
        lcd.print(val);
      }
    Serial1.flush();
    temp=val;
    }
  }
  }

  ///////////////////////////////////////////////////////
  void sendSerial(){
  if(sendData!="")
  {
    Serial1.print(sendData);
    Serial1.print('\r');
    Serial.print("SendData : "+sendData);
    Serial.println('\r');
    sendData = "";
  }
  }

  ///////////////////////////////////////////////////////
  PT_THREAD(taskLED(struct pt* pt))
  {
  static uint32_t ts;

  PT_BEGIN(pt);
  while (1)
  {
    digitalWrite(Status, HIGH);
    //Serial.print(Status);
    PT_DELAY(pt, 150, ts);
  }

  PT_END(pt);
  }

  ///////////////////////////////////////////////////////
  void setup()
  {
  Serial1.begin(115200);
  Serial.begin(115200);

  pinMode(PIN_SC, OUTPUT);
  pinMode(PIN_GC, OUTPUT);
  pinMode(BUZZ, OUTPUT);


  pinMode(but1, INPUT);
  pinMode(but2, INPUT);
  pinMode(but3, INPUT);

  pinMode(PIN_LEDB, OUTPUT);
  pinMode(PIN_LEDR, OUTPUT);
  PT_INIT(&pt_taskLED);

  lcd.begin(16,2);
  lcd.print(val);
  }

  ///////////////////////////////////////////////////////
  void loop()
  {
  taskLED(&pt_taskLED);

  soundCheck();
  gasCheck();
  buttonPress();
  recieveSerial();
  //digitalWrite(Status, HIGH);
  //Serial.print(Status);
  

  }
