#include"PT6312.h"

long CurSeconds = 10000;
int tmp2;
long EthPrice = 3548;  //TODO pull from web
long BTCPrice = 47890; //TODO pull from web

unsigned char DisplayCache[9] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char dict[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

void setup()
{
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  VFD_CLK_H();
  VFD_DIN_H();

  VFD_Init();
  VFD_Display();
  VFD_LED_ON();
  Serial.begin(9600);
}

void loop()
{

  VFD_DATA_DisplayData("ETH", EthPrice);
  VFD_Display();
  delay(5000);
  VFD_DATA_DisplayData("BTC", BTCPrice);
  VFD_Display();
  delay(5000);

}