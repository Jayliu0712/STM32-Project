#include "stm32f10x.h"
#include "motor.h"
#include "usart.h"
#include "stdio.h" //printf��scanf��getchar�Ⱥ���ͷ�ļ�

/********************�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����*************************/
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);			/* ����һ���ֽ����ݵ����� */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);			/* �ȴ�������� */	
	return (ch);
}

/***************�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���***********************/
int fgetc(FILE *f)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);	/* �ȴ������������� */
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
			if(serial_status == 1) //�ɶ�η�������
				{
        flag = 0;					
				GPIO_SetBits(GPIOD,GPIO_Pin_2);
				Motor_Move(data.dirtemp1,data.step1,data.accel1,data.decel1,data.speed1);
				serial_status = 0;
				}	
	}
}
