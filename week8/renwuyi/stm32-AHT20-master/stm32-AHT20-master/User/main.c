#include "delay.h"
#include "usart.h"
#include "bsp_i2c.h"


int main(void)
{	
	delay_init();     //?������o����y3?��??��	  
	uart_init(115200);	 //��??��3?��??��?a115200
	IIC_Init();
		while(1)
	{
		printf("��ʼ���������Եȣ�");
		read_AHT20_once();
		delay_ms(10);
  }
}
