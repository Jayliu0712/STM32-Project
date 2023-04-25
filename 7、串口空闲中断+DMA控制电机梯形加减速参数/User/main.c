#include "stm32f10x.h"
#include "motor.h"
#include "usart.h"
#include "stdio.h" //printf、scanf、getchar等函数头文件

/********************重定向c库函数printf到串口，重定向后可使用printf函数*************************/
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);			/* 发送一个字节数据到串口 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);			/* 等待发送完毕 */	
	return (ch);
}

/***************重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数***********************/
int fgetc(FILE *f)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);	/* 等待串口输入数据 */
	return (int)USART_ReceiveData(USART1);
}

int main(void)
{	
	Motor_Init();
	USART1_Init(115200);
	USART1_DMA_Config();	
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(DMA1_Channel5, ENABLE);
	while(1)
	{	
			if(serial_status == 1) //可多次发送数据
				{
        flag = 0;					
				GPIO_SetBits(GPIOD,GPIO_Pin_2);
				Motor_Move(data.dirtemp1,data.step1,data.accel1,data.decel1,data.speed1);
				serial_status = 0;
				}	
	}
}
