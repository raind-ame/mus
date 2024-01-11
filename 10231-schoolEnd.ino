#include "DHT.h"
#include <Arduino.h>
#include <U8x8lib.h>

#define DHT_pin 3
#define DHT_type DHT11
DHT dht(DHT_pin,DHT_type);
U8X8_SSD1306_128X64_ALT0_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

const int light_pin = A6;
const int led_pin = 4;
int Second = 0,Hour = 0;
float HourTemp[24],dayTemp = 0,total = 0;

void linit() 
{
  for (size_t i = 0;i < 24;i++) HourTemp[i] = 0;
}

void setup() 
{
  linit();
  pinMode(led_pin, OUTPUT);
  pinMode(light_pin,INPUT);
  dht.begin();
  u8x8.begin();
  u8x8.setFlipMode(1);
  Serial.begin(9600);
}

void loop() 
{
  float temp = dht.readTemperature();
  int light = analogRead(light_pin);
  { //OLED temp&humi
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.setCursor(0,0);
    u8x8.print("now  Temp ");
    u8x8.print(temp);
    u8x8.print("C");
    u8x8.setCursor(0,1);
    u8x8.print("24hr Temp ");
    u8x8.print(dayTemp);
    u8x8.print("C");
    u8x8.setCursor(0,2);
    u8x8.print("Second ");
    u8x8.print(Second);
    u8x8.setCursor(0,3);
    u8x8.print("Hour ");
    u8x8.print(Hour);
    u8x8.refreshDisplay();
  }

  { // 測平均時間
    if (Second != 3600)
    {
      Second++;
      delay(710);
    }
    else
    {
      if (Hour != 23)
      {
        Second = 0;
        HourTemp[Hour] = temp;
        Hour++;
        u8x8.setCursor(0,2);
        u8x8.print("Second          ");
      }
      else 
      {
        HourTemp[23] = temp;
        Hour = 0;
        u8x8.setCursor(0,3);
        u8x8.print("Hour            ");
        for (size_t i = 0;i < 24;i++)
        {
          total += HourTemp[i];
        }
        dayTemp = total/24;
        linit();
        total = 0;
      }
    }
  } 

  { //LED亮
    if (abs(temp - dayTemp) >= 3 && light > 500)
    {
      Serial.println("Y");
      digitalWrite(led_pin,HIGH);
    }
    else
    {
      Serial.println("N");
      digitalWrite(led_pin,LOW);
    }
  }
}
