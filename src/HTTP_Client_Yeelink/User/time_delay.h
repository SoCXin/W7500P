#ifndef _TIME_DELAY_H_
#define _TIME_DELAY_H_

#include "wizchip_conf.h"
void delay_init(void);//ϵͳ�δ�ʱ�ӳ�ʼ��
void TimingDelay_Decrement(void);// ϵͳʱ���ж�ִ�к���
void delay_10us(__IO uint32_t time_us);
void delay_ms(__IO uint32_t time_ms);//ms��ʱ
void delay_s(__IO uint32_t time_s);//s��ʱ
void delay_soft_ms( uint32_t nCount);//ms�����ʱ����
#endif

