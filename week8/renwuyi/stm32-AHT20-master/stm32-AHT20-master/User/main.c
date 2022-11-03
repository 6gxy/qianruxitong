#include "delay.h"
#include "usart.h"
#include "bsp_i2c.h"


int main(void)
{	
	delay_init();     //?óê±oˉêy3?ê??ˉ	  
	uart_init(115200);	 //′??ú3?ê??ˉ?a115200
	IIC_Init();
		while(1)
	{
		printf("开始测量，请稍等！");
		read_AHT20_once();
		delay_ms(10);
  }
}
