 #include "bsp_gpio.h"
void led_gpio_init(void)
{
	  GPIO_InitTypeDef GPIO_InitDef;	  
   /* GPIO LED(R) Set */
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_0; // Set to Pin_5 (LED(R))
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT; // Set to Mode Output
    GPIO_Init(GPIOC, &GPIO_InitDef);
    PAD_AFConfig(PAD_PC,GPIO_Pin_0, PAD_AF1); // PAD Config - LED used 2nd Function
	
    /* GPIO LED(G) Set */
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_4; // Set to Pin_5 (LED(G))
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT; // Set to Mode Output
    GPIO_Init(GPIOC, &GPIO_InitDef);
    PAD_AFConfig(PAD_PC,GPIO_Pin_0, PAD_AF1);	// PAD Config - LED used 2nd Function
	  
    /* GPIO LED(B) Set */
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_5; // Set to Pin_5 (LED(B))
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT; // Set to Mode Output
    GPIO_Init(GPIOC, &GPIO_InitDef);
    PAD_AFConfig(PAD_PC,GPIO_Pin_5, PAD_AF1); // PAD Config - LED used 2nd Function
		
    GPIO_SetBits(GPIOC, GPIO_Pin_0); // LED(R) Off
    GPIO_SetBits(GPIOC, GPIO_Pin_4); // LED(G) Off
    GPIO_SetBits(GPIOC, GPIO_Pin_5); // LED(B) Off
}

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




