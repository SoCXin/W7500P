#include "bsp_gpio.h"
#include "time_delay.h"
/**
  * @brief  LED ���ų�ʼ��
  * @param  None
  * @retval None
  */
void LED_GPIO_Configuration(void)
{
	  GPIO_InitTypeDef GPIO_InitDef;	  
   /* PC08 LED(R) PC09 LED(G) PC05 LED(B)*/
    GPIO_InitDef.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_5; // ��������
    GPIO_InitDef.GPIO_Mode =GPIO_Mode_OUT; //�������ģʽ
    GPIO_Init(GPIOC, &GPIO_InitDef);       //�������ģʽ
    PAD_AFConfig(PAD_PC,GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_5, PAD_AF1); // PAD���� Ϊ 2nd Function ģʽ(ȫ��Ϊ��ͨIO��
    GPIO_SetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_5);						// �ر� RGB LED

    GPIO_InitDef.GPIO_Pin = GPIO_Pin_6; // Set to Pin_5 (LED(R))
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT; // Set to Mode Output
    GPIO_Init(GPIOA, &GPIO_InitDef);
    PAD_AFConfig(PAD_PA,GPIO_Pin_6, PAD_AF1); // PAD Config - LED used 2nd Function
	
    /* GPIO LED(G) Set */
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_7; // Set to Pin_5 (LED(G))
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT; // Set to Mode Output
    GPIO_Init(GPIOA, &GPIO_InitDef);
    PAD_AFConfig(PAD_PA,GPIO_Pin_7, PAD_AF1);	// PAD Config - LED used 2nd Function
}	
/**
  * @brief  RGB LED����ѭ����˸(ʹ��ϵͳӲ����ʱʵ��)
  * @param  None
  * @retval None
  */
void blink_led(void)
{
		// LED(RGB) On/off
		delay_ms(500);
		GPIO_ResetBits(GPIOC, GPIO_Pin_0);			
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
		GPIO_ResetBits(GPIOC, GPIO_Pin_5);		  
		delay_ms(500);
		GPIO_SetBits(GPIOC, GPIO_Pin_0);
		GPIO_SetBits(GPIOC, GPIO_Pin_4);
		GPIO_SetBits(GPIOC, GPIO_Pin_5);

		// LED(RG) On/off			
		delay_ms(500);
		GPIO_ResetBits(GPIOC, GPIO_Pin_0);				
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
		delay_ms(500);
		GPIO_SetBits(GPIOC, GPIO_Pin_0);
		GPIO_SetBits(GPIOC, GPIO_Pin_4);

		// LED(GB) On/off
		delay_ms(500);
		GPIO_ResetBits(GPIOC, GPIO_Pin_0);				
		GPIO_ResetBits(GPIOC, GPIO_Pin_5);			
		delay_ms(500);
		GPIO_SetBits(GPIOC, GPIO_Pin_0);				
		GPIO_SetBits(GPIOC, GPIO_Pin_5);

		// LED(BR) On/off
		delay_ms(500);
		GPIO_ResetBits(GPIOC, GPIO_Pin_5);				
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);			
		delay_ms(500);
		GPIO_SetBits(GPIOC, GPIO_Pin_5);				
		GPIO_SetBits(GPIOC, GPIO_Pin_4);

		// LED(R) On/off
		delay_ms(500);
		GPIO_ResetBits(GPIOC, GPIO_Pin_0);
		delay_ms(500);
		GPIO_SetBits(GPIOC, GPIO_Pin_0);

		// LED(G) On/off
		delay_ms(500);
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
		delay_ms(500);
		GPIO_SetBits(GPIOC, GPIO_Pin_4);

		// LED(B) On/off
		delay_ms(500);
		GPIO_ResetBits(GPIOC, GPIO_Pin_5);
		delay_ms(500);
		GPIO_SetBits(GPIOC, GPIO_Pin_5);			
}

/**
  * @brief  RGB LED����ѭ����˸(ʹ������ʱ����ʵ��)
  * @param  None
  * @retval None
  */
void blink1_led(void)
{
		// LED(RGB) On/off
		delay_soft_ms(500);
		GPIO_ResetBits(GPIOC, GPIO_Pin_0);			
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
		GPIO_ResetBits(GPIOC, GPIO_Pin_5);		  
		delay_soft_ms(500);
		GPIO_SetBits(GPIOC, GPIO_Pin_0);
		GPIO_SetBits(GPIOC, GPIO_Pin_4);
		GPIO_SetBits(GPIOC, GPIO_Pin_5);

		// LED(RG) On/off			
		delay_soft_ms(500);
		GPIO_ResetBits(GPIOC, GPIO_Pin_0);				
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
		delay_soft_ms(500);
		GPIO_SetBits(GPIOC, GPIO_Pin_0);
		GPIO_SetBits(GPIOC, GPIO_Pin_4);

		// LED(GB) On/off
		delay_soft_ms(500);
		GPIO_ResetBits(GPIOC, GPIO_Pin_0);				
		GPIO_ResetBits(GPIOC, GPIO_Pin_5);			
		delay_soft_ms(500);
		GPIO_SetBits(GPIOC, GPIO_Pin_0);				
		GPIO_SetBits(GPIOC, GPIO_Pin_5);

		// LED(BR) On/off
		delay_soft_ms(500);
		GPIO_ResetBits(GPIOC, GPIO_Pin_5);				
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);			
		delay_soft_ms(500);
		GPIO_SetBits(GPIOC, GPIO_Pin_5);				
		GPIO_SetBits(GPIOC, GPIO_Pin_4);

		// LED(R) On/off
		delay_soft_ms(500);
		GPIO_ResetBits(GPIOC, GPIO_Pin_0);
		delay_soft_ms(500);
		GPIO_SetBits(GPIOC, GPIO_Pin_0);

		// LED(G) On/off
		delay_soft_ms(500);
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
		delay_soft_ms(500);
		GPIO_SetBits(GPIOC, GPIO_Pin_4);

		// LED(B) On/off
		delay_soft_ms(500);
		GPIO_ResetBits(GPIOC, GPIO_Pin_5);
		delay_soft_ms(500);
		GPIO_SetBits(GPIOC, GPIO_Pin_5);			
}
/**
  * @brief  ��Ʒ�ת
  * @param  None
  * @retval None
  */
void redled_toggle(void)
{
    if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_0) != (uint32_t)Bit_RESET)//RED ���ǹرյ�
    {			  
	    GPIO_ResetBits(GPIOC,GPIO_Pin_0);	//��RED LED			
    }
    else
    {
        GPIO_SetBits(GPIOC,GPIO_Pin_0);	//�ر�RED LED		
    }
}


