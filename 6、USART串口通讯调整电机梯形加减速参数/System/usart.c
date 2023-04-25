#include "stm32f10x.h"
#include "stdio.h" //printf、scanf、getchar等函数头文件
#include "usart.h"
#include "delay.h"

struct Motormove_Data data;
int serial_status = 0;

void USART1_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //查数据手册，STM32F103x PA9 PA10为USART1_TX USART1_RX复用端口
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
		
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口
		
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//中断配合接收数据使用
//  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//空闲中断
  USART_Cmd(USART1, ENABLE);                   //使能串口 
}

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

void USART1_IRQHandler(void)
{
	static unsigned int i=0;
	int temp;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
{
	i++;
	switch(i)  //修改电机运动参数
		{
		case 1:
		data.dirtemp1 = USART_ReceiveData(USART1);//读取接收到的数据
		printf("dirtemp:%d\n",data.dirtemp1);	//把收到的数据发送回电脑
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
		if(i == 5) //可多次发送数据
		{	
			i=0;
			serial_status = 1;
		}
}
} 
