#ifndef _TIME_DELAY_H_
#define _TIME_DELAY_H_

#include "wizchip_conf.h"
void delay_init(void);//系统滴答时钟初始化
void TimingDelay_Decrement(void);// 系统时钟中断执行函数
void delay_10us(__IO uint32_t time_us);
void delay_ms(__IO uint32_t time_ms);//ms延时
void delay_s(__IO uint32_t time_s);//s延时
void delay_soft_ms( uint32_t nCount);//ms软件延时函数
#endif

