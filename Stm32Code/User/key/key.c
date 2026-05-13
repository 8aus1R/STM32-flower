#include "stm32f10x.h"
#include "key.h"
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK精英STM32开发板
//按键驱动代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

u8 isKey1 = 0;
u8 isKey2 = 0;
u8 isKey3 = 0;
u8 isKey4 = 0;
u8 isKey5 = 0;
u8 isKey6 = 0;
u8 isKey7 = 0;
u8 isKey8 = 0;

//
/*
******按键初始化函数*****************************************
*	功能:	按键初始化函数
*	入口参数:	无
*	返回参数:	无
*	说明:		KEY1-PB14		KEY2-PB12   KEY3-PB15    KEY4-PB13
************************************************************
*/
void KEY_Init(void) //IO初始化
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY7_GPIO_CLK,ENABLE);

    GPIO_InitStructure.GPIO_Pin  = KEY1_GPIO_PIN|KEY2_GPIO_PIN|KEY3_GPIO_PIN|KEY4_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入

    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);//
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);//
    GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStructure);//
    GPIO_Init(KEY4_GPIO_PORT, &GPIO_InitStructure);//
    //
    
//    GPIO_InitStructure.GPIO_Pin  = KEY5_GPIO_PIN|KEY6_GPIO_PIN|KEY7_GPIO_PIN|KEY8_GPIO_PIN;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
    
//    GPIO_Init(KEY5_GPIO_PORT, &GPIO_InitStructure);//
//    GPIO_Init(KEY6_GPIO_PORT, &GPIO_InitStructure);//
//    GPIO_Init(KEY7_GPIO_PORT, &GPIO_InitStructure);//
//    GPIO_Init(KEY8_GPIO_PORT, &GPIO_InitStructure);//
//    GPIO_SetBits(KEY5_GPIO_PORT,KEY5_GPIO_PIN);
//    GPIO_SetBits(KEY6_GPIO_PORT,KEY6_GPIO_PIN);
//    GPIO_SetBits(KEY7_GPIO_PORT,KEY7_GPIO_PIN);
//    GPIO_SetBits(KEY8_GPIO_PORT,KEY8_GPIO_PIN);
}


void Key1Press(void)
{
    if(isKey1 == 0)
        isKey1=1;
}

void Key2Press(void)
{
    if(isKey2 == 0)
        isKey2=1;

}

void Key3Press(void)
{
    if(isKey3 == 0)
        isKey3=1;

}

void Key4Press(void)
{
    if(isKey4 == 0)
        isKey4=1;

}





/*
******按键扫描函数*****************************************
*	功能:	按键扫描函数
*	入口参数:	无
*	返回参数:	无
*	说明:		KEY1-PB14		KEY2-PB12   KEY3-PB15    KEY4-PB13
************************************************************
*/
void KeyScan(void)
{
    static u8 key_up = 1; 
    if (key_up && (KEY1 == 0 || KEY2 == 0 || KEY3 == 0 || KEY4 == 0)) 
    {
        delay_ms(10); 
        key_up = 0;  

        if (KEY1 == 0) Key1Press();
        else if (KEY2 == 0) Key2Press();
        else if (KEY3 == 0) Key3Press();
        else if (KEY4 == 0) Key4Press();
    } 

    else if (KEY1 == 1 && KEY2 == 1 && KEY3 == 1 && KEY4 == 1) 
    {
        key_up = 1; 
    }
}
