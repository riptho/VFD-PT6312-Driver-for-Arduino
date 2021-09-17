/******************* (C) COPYRIGHT 2019 TerayTechEnterprise *****START OF FILE****/
//Modified by Riptho 2021

#include "Arduino.h"

#define VFD_CLK_H() digitalWrite(6, LOW);
#define VFD_CLK_L() digitalWrite(6, HIGH);

#define VFD_DIN_H() digitalWrite(7, HIGH);
#define VFD_DIN_L() digitalWrite(7, LOW);

#define VFD_STB_H() digitalWrite(8, HIGH);
#define VFD_STB_L() digitalWrite(8, LOW);

extern long CurSeconds;

extern unsigned char vfdmap[9];
int alphabetDict[28] = {119,124,57,94,121,113,61,116,48,30,117,56,21,55,63,115,103,51,109,120,62,46,42,118,110,75}; //A-Z dictionary lookup

extern unsigned char dict[];
extern unsigned char DisplayCache[9];

#define MODE 0x05  // VFD 显示模式设定 ( 9 digit*13 segment)
#define LIGHT 0x05 // VFD 亮度设定 (0x00-0x07)
#define ON 0x08    // 打开显示
#define OFF 0x00   // 关闭显示

/******************************函数定义部分**************************/

void Write_PT6312(unsigned char dat)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        VFD_CLK_H();
        if (dat & 0x01)
        {
            VFD_DIN_H();
        }
        else
        {
            VFD_DIN_L();
        }
        dat >>= 1;
        VFD_CLK_L();
    }
    delayMicroseconds(6);
}

void send_cs(void)
{
    VFD_STB_H();
    VFD_STB_L();
}

//VFD初始化
void VFD_Init(void)
{
    int i;
    send_cs();
    Write_PT6312(MODE); // 段位设置
    send_cs();
    Write_PT6312(0x40); // 设定数据
    send_cs();
    Write_PT6312(0xc0); // Set Address
    for (i = 0; i < 9; i++)
    {
        Write_PT6312(0x00); // Transfer display data low byte
        Write_PT6312(0x00); // High byte data transmission display
    }
    send_cs();
    Write_PT6312(0x80 | ON | LIGHT); // Display Control
    VFD_STB_H();
    VFD_DIN_H();
}

//显示测试
void VFD_Display_Test(void)
{
    int i;
    send_cs();
    Write_PT6312(0xc0); // Set Address
    for (i = 0; i < 9; i++)
    {
        Write_PT6312(vfdmap[i]); // Transfer display data low byte
        Write_PT6312(0x00);      // High byte data transmission display
    }
    send_cs();
    Write_PT6312(0x80 | ON | LIGHT); // Display Control
    VFD_STB_H();
}

void VFD_LED_ON(void)
{
    send_cs();
    Write_PT6312(0x41); // 设定LED
    Write_PT6312(0x0C); // led1&LED2
    VFD_STB_H();
}

void VFD_LED_OFF(void)
{
    send_cs();
    Write_PT6312(0x41); // 设定LED
    Write_PT6312(0x0F); // led1&LED2
    VFD_STB_H();
}

void VFD_Display(void)
{
    int i;
    unsigned char *p = DisplayCache;
    send_cs();
    Write_PT6312(0xc0);
    for (i = 0; i < 9; i++)
    {
        Write_PT6312(*p++);
        Write_PT6312(0x00);
    }
    send_cs();
    Write_PT6312(0x80 | ON | LIGHT);
    VFD_STB_H();
    VFD_DIN_H();
}

int CharLookup(char input) //decode chars for 7 segment display
{
    if(input > 96 ){
        return alphabetDict[(input - 97)];  //decode lowercase chars
    }
    else if(input < 91){
        return alphabetDict[(input - 65)];  //decode uppercase chars
    }

    else
        return 0;


}

void VFD_DATA_DisplayData(char ticker[], long value) //method for displaying stock tickers and their current value. {XXX 99999} display.
{
    unsigned char tmp;
    if (value > 99999 || sizeof(ticker) > 3)
    {
        return;
    }
    else
    {
        DisplayCache[0] = CharLookup(ticker[0]);
        DisplayCache[1] = CharLookup(ticker[1]);
        DisplayCache[2] = CharLookup(ticker[2]);
        DisplayCache[3] = 0;
        tmp = value % 100000 / 10000;
        if(tmp == 0){DisplayCache[4] = 0x00;}
        else{DisplayCache[4] = dict[tmp];}
        tmp = value % 10000 / 1000;
        if(tmp == 0 && DisplayCache[4] == 0){DisplayCache[5] = 0x00;}
        else{DisplayCache[5] = (dict[tmp]^128);}                        //xor'd for comma placement 
        tmp = value % 1000 / 100;
        if(tmp == 0 && DisplayCache[5] == 0){DisplayCache[6] = 0x00;}
        else{DisplayCache[6] = dict[tmp];}
        tmp = value % 100 / 10;
        if(tmp == 0 && DisplayCache[6] == 0){DisplayCache[7] = 0x00;}
        else{DisplayCache[7] = dict[tmp];}
        tmp = value % 10;
        if(tmp == 0 && DisplayCache[7] == 0){DisplayCache[8] = 0x00;}
        else{DisplayCache[8] = dict[tmp];}
    }
}

void VFD_DATA_2_DisplayCache(void)  //TODO modify to accept up to 9 chars to be displayed both alpha and numeric.
{
    unsigned char tmp;
    tmp = CurSeconds / 100000000;
    if (tmp == 0)
    {
        DisplayCache[0] = 0x00;
    }
    else
    {
        DisplayCache[0] = dict[tmp];
    }
    tmp = CurSeconds % 100000000 / 10000000;
    if (tmp == 0 && DisplayCache[0] == 0)
    {
        DisplayCache[1] = 0x00;
    }
    else
    {
        DisplayCache[1] = dict[tmp];
    }
    tmp = CurSeconds % 10000000 / 1000000;
    if (tmp == 0 && DisplayCache[1] == 0)
    {
        DisplayCache[2] = 0x00;
    }
    else
    {
        DisplayCache[2] = dict[tmp];
    }
    tmp = CurSeconds % 1000000 / 100000;
    if (tmp == 0 && DisplayCache[2] == 0)
    {
        DisplayCache[3] = 0x00;
    }
    else
    {
        DisplayCache[3] = dict[tmp];
    }
    tmp = CurSeconds % 100000 / 10000;

    if (tmp == 0 && DisplayCache[3] == 0)
    {
        DisplayCache[4] = 0x00;
    }
    else
    {
        DisplayCache[4] = dict[tmp];
    }
    tmp = CurSeconds % 10000 / 1000;
    if (tmp == 0 && DisplayCache[4] == 0)
    {
        DisplayCache[5] = 0x00;
    }
    else
    {
        DisplayCache[5] = dict[tmp];
    }
    tmp = CurSeconds % 1000 / 100;
    if (tmp == 0 && DisplayCache[5] == 0)
    {
        DisplayCache[6] = 0x00;
    }
    else
    {
        DisplayCache[6] = dict[tmp];
    }
    tmp = CurSeconds % 100 / 10;
    if (tmp == 0 && DisplayCache[6] == 0)
    {
        DisplayCache[7] = 0x00;
    }
    else
    {
        DisplayCache[7] = dict[tmp];
    }
    tmp = CurSeconds % 10;
    if (tmp == 0 && DisplayCache[7] == 0)
    {
        DisplayCache[8] = 0x00;
    }
    else
    {
        DisplayCache[8] = dict[tmp];
    }
}

/**********************函数定义结束*************************/