//-------------------------------------------------------------------
// nRF24 echo client
//------------------------------ written by iizuka   Japan ----------
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Wire.h>
#include <MPL115A2.h>
#include <Sensirion.h>

// コンストラクタ呼び出し CEピンとCSピン番号の設定
RF24 radio(10,9);
#define LED_PIN (8)
const uint8_t dataPin  =  7;
const uint8_t clockPin =  6;
Sensirion tempSensor = Sensirion(dataPin, clockPin);
float temperature;
float humidity;
float dewpoint;
float p;

void setup(void)
{
  Serial.begin(9600);
  Serial.write("start client");

  // 通信開始
  radio.begin();

  // リトライ回数の設定
  radio.setRetries(15,15);

  // 書き込み用のパイプを開く
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  // 読み込み用のパイプを開く
  radio.openReadingPipe(1, 0xF0F0F0F0D2LL);
  radio.startListening();


  MPL115A2.begin();
  p=MPL115A2.read();
}
void xSend(char* s)
{
  //文字列の送信
  digitalWrite(LED_PIN, 1);
  radio.stopListening();
  while(*s!=0){
    xSendChar(*s++);
  }
  radio.startListening();
  digitalWrite(LED_PIN, 0);  
}
void xSendChar(char c)
{
  int ok;
  // サーバーに１文字送信する
  ok = radio.write( &c, sizeof(char) );
  delay(10);
/*
  digitalWrite(LED_PIN, 1);
  delay(1);
  digitalWrite(LED_PIN, 0);  
*/
}

void loop(void)
{
  char c;
  bool ok;
  char s[50];
  
  if(radio.available()){
    bool done=false;
    Serial.write("radio available.\r\n");
    tempSensor.measure(&temperature, &humidity, &dewpoint);

    while(!done){
      done=radio.read( &c, sizeof(char) );
      if(c=='t'){
        sprintf(s,"t,%d,h,%d,d,%d,p,%d\r\n"
        ,(int)(temperature*10),(int)(humidity*10),(int)(dewpoint*10),(int)(p*10));
        xSend(s);
        Serial.write(s);
        //delay(1000);
      }
    }
  }
//  delay(100);
//  Serial.write("cli test\r\n");
  
  
  p=p*0.9+(MPL115A2.read()*0.1);
  //Serial.print(p);
  //Serial.println(",");
}
//-------- end of program -----------------------------------------
