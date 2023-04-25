#include "stm32f10x.h"
#include "stdio.h" //printf��scanf��getchar��vsnprintf�Ⱥ���ͷ�ļ�
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "usart.h"
#include "delay.h"

char 	   ReceiveData[DATA_SIZE];
uint8_t  SendBuffer[SIZE];

struct Motormove_Data data;
unsigned int flag;
int serial_status = 0;
int temp = 0;

void USART1_DMA_Config(void)//����ͨ��DMA��ʼ����
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	DMA_InitTypeDef  DMA_InitStruct;
	DMA_InitStruct.DMA_PeripheralBaseAddr 	= (uint32_t)(USART1_BASE + 0x04); //�����ַ:USART1�������ݼĴ�����ַ��USART1_BASE + 0x04
	DMA_InitStruct.DMA_MemoryBaseAddr 		= (uint32_t)ReceiveData;            //�ڴ��ַ
	DMA_InitStruct.DMA_DIR 					= DMA_DIR_PeripheralSRC; 			            //���䷽��
	DMA_InitStruct.DMA_BufferSize 			= DATA_SIZE;                          //��������ݶ���   
	DMA_InitStruct.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;          //�����ַ�Ƿ����
	DMA_InitStruct.DMA_MemoryInc			= DMA_MemoryInc_Enable;                 //�ڴ��ַ�Ƿ����
	DMA_InitStruct.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;    //�������ݿ��
	DMA_InitStruct.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte;          //�ڴ����ݿ��
	DMA_InitStruct.DMA_Mode 				= DMA_Mode_Normal;                        //ģʽ
	DMA_InitStruct.DMA_Priority 			= DMA_Priority_High;                    //���ȼ�
	DMA_InitStruct.DMA_M2M 					= DMA_M2M_Disable;                        //�Ƿ�Ϊ�ڴ浽�ڴ�ķ�ʽ
 
	DMA_Init(DMA1_Channel5, &DMA_InitStruct); //��ο��ֲ�DMA���֡�����ͨ��DMA����һ��������֪USART1�Ľ��ն�������DMA1��ͨ��5
	
	DMA_ClearFlag(DMA1_FLAG_TC5);            //���־λ������һֱ���ж�
	
	//	DMA_Cmd(DMA1_Channel5, ENABLE);
}		
 

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
	USART_InitStructure.USART_BaudRate = bound;
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
	
	USART_ClearFlag(USART1, USART_FLAG_IDLE | USART_FLAG_RXNE);//���״̬��־λ��
	
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�ж���Ͻ�������ʹ��
  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//�����ж�
  USART_Cmd(USART1, ENABLE);                   //ʹ�ܴ��� 
}

/***************************************�������麯��*************************************************/
void Usart_SendBuffer(uint8_t * sendbuffer)
{
	uint16_t len = strlen((char *)sendbuffer); //����ַ����ĳ���
	uint16_t i = 0;
	while(i < len)                            //һ�η���һ���ַ���ֱ�������ַ����������
	{
		USART_SendData(USART1, (uint8_t) sendbuffer[i]); //���ַ���һ���ַ�һ���ַ���������
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		/*
		USART_FLAG_TXE�������ݼĴ����ձ�־λ ��USART_FLAG_TC������ɱ�־λ��,
		���������ݼĴ���������ݱ�ȫ��ȡ��ʱ��
		�üĴ����ǿյģ���ô�ñ�־λ�ͻᱻ��1��
		���ݻ�û��ȡ�꣬i�ͼ���++��ֱ�����ݱ�ȡ��
		��ֹ���ݶ�ʧ�������ɺ�����ɾ����֤һ�£�
		*/
		i++;
	}
}

/***************************************�������ݺ���*************************************************/
void Usart_Printf(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	vsnprintf((char *)SendBuffer,SIZE,format,args);//vsnprintf�������ܣ����ɱ������ʽ�������һ���ַ����顣
	va_end(args);
	Usart_SendBuffer(SendBuffer);
	memset(SendBuffer, 0, SIZE); //��ʼ���������ݶ���0����ˣ�
}

void USART1_IRQHandler(void)//�����жϷ�����
{
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)//�ж��Ƿ����˿����ж�
	{
		DMA_Cmd(DMA1_Channel5, DISABLE);
//		buffersize = (DATA_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5));//��ȡ�������ݵĳ���
		sscanf( ReceiveData,"%d",&temp); //���ַ�����ReceiveData�е�������"%d"��ʽ���뵽temp�С�
		if(temp != 0) 
		{
			flag++;
		}			
		/*
		û�����if�жϵĻ�������һ��ʼ��δ����ַ���ʱ��
		״̬�ǿ��еģ����temp��ֵ0��������Ƶ���������
		*/
					switch(flag)
			{
			case 1:
				serial_status = 0;
				data.dirtemp1 = temp;//��ȡ���յ�������
			printf("\r\ndirtemp:%d",data.dirtemp1);//���յ������ݷ��ͻص���
			break;
			case 2:
				data.step1 = temp;
			printf("\r\nstep:%d",data.step1); 
			break;
			case 3:
				data.accel1 = temp;
			printf("\r\naccel:%d",data.accel1); 
			break;
			case 4:
				data.decel1 = temp;
			printf("\r\ndecel:%d",data.decel1); 
			break;
			case 5:
				data.speed1 = temp;
			printf("\r\nspeed:%d",data.speed1);
			printf("\r\nMotor Running...");
			serial_status = 1; //����ο����жϣ����������ֵ��ϣ��������״̬������ֵ1
			break;
			default:
				break;
			}		
//		Usart_Printf("%d\n",temp);//�����յ������ݷ��ͳ�ȥ;
		memset(ReceiveData, 0, DATA_SIZE);//����������ReceiveData���
		DMA_SetCurrDataCounter(DMA1_Channel5, DATA_SIZE);//���õ�ǰͨ��ʣ��������
		
//		DMA_ClearFlag(DMA1_FLAG_TC5)
		DMA_DeInit(DMA1_Channel5);//��DMA���³�ʼ��
		USART1_DMA_Config();
		USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
		DMA_Cmd(DMA1_Channel5, ENABLE);
		
		USART_ReceiveData(USART1);
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);
		}
}

