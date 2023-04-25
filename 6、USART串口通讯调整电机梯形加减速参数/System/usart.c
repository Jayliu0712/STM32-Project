#include "stm32f10x.h"
#include "stdio.h" //printf��scanf��getchar�Ⱥ���ͷ�ļ�
#include "usart.h"
#include "delay.h"

struct Motormove_Data data;
int serial_status = 0;

void USART1_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //�������ֲᣬSTM32F103x PA9 PA10ΪUSART1_TX USART1_RX���ö˿�
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
		
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������
		
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
		
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�ж���Ͻ�������ʹ��
//  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//�����ж�
  USART_Cmd(USART1, ENABLE);                   //ʹ�ܴ��� 
}

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

void USART1_IRQHandler(void)
{
	static unsigned int i=0;
	int temp;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
{
	i++;
	switch(i)  //�޸ĵ���˶�����
		{
		case 1:
		data.dirtemp1 = USART_ReceiveData(USART1);//��ȡ���յ�������
		printf("dirtemp:%d\n",data.dirtemp1);	//���յ������ݷ��ͻص���
		break;
		case 2:
			temp = USART_ReceiveData(USART1);
			data.step1 = 1000*temp;
		printf("step:%d\n",data.step1); 
		break;
		case 3:
			temp = USART_ReceiveData(USART1);
			data.accel1 = 1000*temp;
		printf("accel:%d\n",data.accel1); 
		break;
		case 4:
			temp = USART_ReceiveData(USART1);
			data.decel1 = 1000*temp;
		printf("decel:%d\n",data.decel1); 
		break;
		case 5:
			temp = USART_ReceiveData(USART1);
			data.speed1 = 1000*temp;
		printf("speed:%d\n",data.speed1);
		Delay_us(5);
		printf("Motor Running...\n");
		break;
		default:
			break;
		}
		if(i == 5) //�ɶ�η�������
		{	
			i=0;
			serial_status = 1;
		}
}
} 
