/*
**************************************************************************************************
* @file    		time_delay.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-xx-xx
* @brief  		����MCU��ϵͳʱ����ʱ����
**************************************************************************************************
*/
#include "wizchip_conf.h"
#include "time_delay.h"
uint32_t TimingDelay;

/**
  * @brief  ϵͳ�δ�ʱ�ӳ�ʼ������.
  * @param  ��
  * @retval ��
  */
void delay_init(void)
{
    SysTick_Config((GetSystemClock()/1000));    	/* ϵͳ�δ�ʱ������ */
}

/**
  * @brief  ϵͳʱ���ж���ʱִ�к���.
  * @param  ��
  * @retval ��
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

/**
  * @brief  ms��ʱ����.
  * @param  time_us: ��Ҫ��ʱmsʱ��.
  * @retval ��
  */
void delay_ms(__IO uint32_t time_ms)
{
  TimingDelay = time_ms;
  while(TimingDelay != 0);
}

/**
  * @brief  s��ʱ����.
  * @param  time_s: ��Ҫ��ʱmsʱ��.
  * @retval ��
  */
void delay_s(__IO uint32_t time_s)
{
	for(;time_s>0;time_s--)
  delay_ms(1000);
}


/**
  * @brief  ms�����ʱ����
  * @param  nCount: ��Ҫ��ʱmsʱ��.
  * @retval None
  */
void delay_soft_ms( uint32_t nCount)
{
    volatile uint32_t delay = nCount * 2500; // approximate loops per ms at 24 MHz, Debug config
    for(; delay != 0; delay--)
        __NOP();
}
