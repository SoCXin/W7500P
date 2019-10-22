/**
******************************************************************************
* @file   		http_client.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-xx-xx
* @brief   		http_client ��ʾ���� 
******************************************************************************/
#include "wizchip_conf.h"
#include "ip_config.h"
#include "time_delay.h"
#include "httpc.h"
#include "dht11.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint8_t temp_rh[2];//p[0]��ʪ�� p[1]���¶�
DHT11_Data_TypeDef DHT11_Data;
char  postT[]={//�ύ�¶�
"POST /v1.0/device/15738/sensor/27051/datapoints HTTP/1.1\r\n"
"Host:api.yeelink.net\r\n"
"Accept:*/*\r\n"
"U-ApiKey:fea57b9cc1ed5ad34d48430785b44e8f\r\n"
"Content-Length:12\r\n"
"Content-Type:application/x-www-form-urlencoded\r\n"
"Connection:close\r\n"
"\r\n"
"{\"value\":xx}\r\n"
};//xx���������ֵ

char  postH[]={//�ύʪ��
"POST /v1.0/device/15738/sensor/27053/datapoints HTTP/1.1\r\n"
"Host:api.yeelink.net\r\n"
"Accept:*/*\r\n"
"U-ApiKey:fea57b9cc1ed5ad34d48430785b44e8f\r\n"
"Content-Length:12\r\n"
"Content-Type:application/x-www-form-urlencoded\r\n"
"Connection:close\r\n"
"\r\n"
"{\"value\":xx}\r\n"
};//xx����
char *post[]={postH,postT};

uint8_t yeelink_server_ip[4]={42, 96, 164, 52};			//api.yeelink.net  ��ip��ַ
uint8_t yeelink_port=80;														//api.yeelink.net  �Ķ˿ں�	

/**
*@brief		ִ��http client������
*@param		sn:socket number , port_httpc: �ͻ���socket�˿ں�
*@return	��
*/
void httpclient(uint8_t sn,uint16_t port_httpc)
{
   int8_t ret=0;
		uint8_t t=0;  
   static uint8_t i=0;
	uint8_t Buffer[1024]={0};
	
   switch(getSn_SR(sn))																/*��ȡsocket��״̬*/
   {
		   case SOCK_INIT :				/*socket��ʼ�����*/
				connect(sn,yeelink_server_ip,80);	
      break;
      case SOCK_ESTABLISHED :													//socket���ӽ����ɹ�
         if(getSn_IR(sn) & Sn_IR_CON)
         {
            setSn_IR(sn,Sn_IR_CON);										//����жϱ�־λ
         }
				if( Read_DHT11(&DHT11_Data))									
				{
					temp_rh[0]=DHT11_Data.humi_int;								//��ȡ����ʪ��ֵ		
					temp_rh[1]=DHT11_Data.temp_int;								//��ȡ�����¶�ֵ	
					memcpy(Buffer,post[i],strlen(post[i]));
					Buffer[231]=temp_rh[i]/10+0x30;		  
					Buffer[232]=temp_rh[i]%10+0x30;
					send(sn,Buffer,sizeof(Buffer));								//W5500�� Yeelink��������������
					i=!i;
					if(t%5000==0)printf("RH:%d, Temp:%d\r\n", temp_rh[0], temp_rh[1]);
					else t++;
				}
      break;
      case SOCK_CLOSE_WAIT :														//socket�ȴ��ر�״̬
					close(sn);
      break;
      case SOCK_CLOSED:																	//socket�ر�״̬
         socket(sn, Sn_MR_TCP, port_httpc,0x01);				//����socket
      break;
      default:
         break;
   }
}
