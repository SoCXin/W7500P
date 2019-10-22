/*
**************************************************************************************************
* @file    		time_delay.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-xx-xx
* @brief  		配置MCU的系统时钟延时函数
**************************************************************************************************
*/
#include "wizchip_conf.h"
#include "time_delay.h"
uint32_t TimingDelay;

/**
  * @brief  系统滴答时钟初始化函数.
  * @param  无
  * @retval 无
  */
void delay_init(void)
{
    SysTick_Config((GetSystemClock()/1000));    	/* 系统滴答时钟配置 */
}

/**
  * @brief  系统时钟中断延时执行函数.
  * @param  无
  * @retval 无
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

/**
  * @brief  ms延时函数.
  * @param  time_us: 需要延时ms时间.
  * @retval 无
  */
void delay_ms(__IO uint32_t time_ms)
{
  TimingDelay = time_ms;
  while(TimingDelay != 0);
}

/**
  * @brief  s延时函数.
  * @param  time_s: 需要延时ms时间.
  * @retval 无
  */
void delay_s(__IO uint32_t time_s)
{
	for(;time_s>0;time_s--)
  delay_ms(1000);
}


/**
  * @brief  ms软件延时函数
  * @param  nCount: 需要延时ms时间.
  * @retval None
  */
void delay_soft_ms( uint32_t nCount)
{
    volatile uint32_t delay = nCount * 2500; // approximate loops per ms at 24 MHz, Debug config
    for(; delay != 0; delay--)
        __NOP();
}
