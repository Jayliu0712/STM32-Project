#include "stm32f10x.h"
#include "stdio.h" //printf、scanf、getchar、vsnprintf等函数头文件
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

void USART1_DMA_Config(void)//接收通道DMA初始化；
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	DMA_InitTypeDef  DMA_InitStruct;
	DMA_InitStruct.DMA_PeripheralBaseAddr 	= (uint32_t)(USART1_BASE + 0x04); //外设地址:USART1发送数据寄存器地址是USART1_BASE + 0x04
	DMA_InitStruct.DMA_MemoryBaseAddr 		= (uint32_t)ReceiveData;            //内存地址
	DMA_InitStruct.DMA_DIR 					= DMA_DIR_PeripheralSRC; 			            //传输方向
	DMA_InitStruct.DMA_BufferSize 			= DATA_SIZE;                          //传输的数据多少   
	DMA_InitStruct.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;          //外设地址是否递增
	DMA_InitStruct.DMA_MemoryInc			= DMA_MemoryInc_Enable;                 //内存地址是否递增
	DMA_InitStruct.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;    //外设数据宽度
	DMA_InitStruct.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte;          //内存数据宽度
	DMA_InitStruct.DMA_Mode 				= DMA_Mode_Normal;                        //模式
	DMA_InitStruct.DMA_Priority 			= DMA_Priority_High;                    //优先级
	DMA_InitStruct.DMA_M2M 					= DMA_M2M_Disable;                        //是否为内存到内存的方式
 
	DMA_Init(DMA1_Channel5, &DMA_InitStruct); //查参考手册DMA部分《各个通道DMA请求一览》，可知USART1的接收端请求在DMA1的通道5
	
	DMA_ClearFlag(DMA1_FLAG_TC5);            //清标志位，否则一直进中断
	
	//	DMA_Cmd(DMA1_Channel5, ENABLE);
}		
 

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
	USART_InitStructure.USART_BaudRate = bound;
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
	
	USART_ClearFlag(USART1, USART_FLAG_IDLE | USART_FLAG_RXNE);//清除状态标志位；
	
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//中断配合接收数据使用
  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//空闲中断
  USART_Cmd(USART1, ENABLE);                   //使能串口 
}

/***************************************发送数组函数*************************************************/
void Usart_SendBuffer(uint8_t * sendbuffer)
{
	uint16_t len = strlen((char *)sendbuffer); //算出字符串的长度
	uint16_t i = 0;
	while(i < len)                            //一次发送一个字符，直到整个字符串发送完毕
	{
		USART_SendData(USART1, (uint8_t) sendbuffer[i]); //将字符串一个字符一个字符存入数组
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		/*
		USART_FLAG_TXE发送数据寄存器空标志位 （USART_FLAG_TC发送完成标志位）,
		当发送数据寄存器里的数据被全部取完时，
		该寄存器是空的，那么该标志位就会被置1。
		数据还没被取完，i就继续++，直到数据被取完
		防止数据丢失（设计完成后把这句删了验证一下）
		*/
		i++;
	}
}

/***************************************发送数据函数*************************************************/
void Usart_Printf(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	vsnprintf((char *)SendBuffer,SIZE,format,args);//vsnprintf函数功能：将可变参数格式化输出到一个字符数组。
	va_end(args);
	Usart_SendBuffer(SendBuffer);
	memset(SendBuffer, 0, SIZE); //初始化（将数据都用0替代了）
}

void USART1_IRQHandler(void)//串口中断服务函数
{
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)//判断是否发生了空闲中断
	{
		DMA_Cmd(DMA1_Channel5, DISABLE);
//		buffersize = (DATA_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5));//获取接收数据的长度
		sscanf( ReceiveData,"%d",&temp); //把字符数组ReceiveData中的内容以"%d"格式输入到temp中。
		if(temp != 0) 
		{
			flag++;
		}			
		/*
		没有这个if判断的话，程序一开始起动未输出字符串时，
		状态是空闲的，会把temp初值0输出到控制电机方向参数
		*/
					switch(flag)
			{
			case 1:
				serial_status = 0;
				data.dirtemp1 = temp;//读取接收到的数据
			printf("\r\ndirtemp:%d",data.dirtemp1);//把收到的数据发送回电脑
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
			serial_status = 1; //第五次空闲中断，五个参数赋值完毕，将电机起动状态参数赋值1
			break;
			default:
				break;
			}		
//		Usart_Printf("%d\n",temp);//将接收到的数据发送出去;
		memset(ReceiveData, 0, DATA_SIZE);//将接收数组ReceiveData清除
		DMA_SetCurrDataCounter(DMA1_Channel5, DATA_SIZE);//设置当前通道剩余数据量
		
//		DMA_ClearFlag(DMA1_FLAG_TC5)
		DMA_DeInit(DMA1_Channel5);//将DMA重新初始化
		USART1_DMA_Config();
		USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
		DMA_Cmd(DMA1_Channel5, ENABLE);
		
		USART_ReceiveData(USART1);
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);
		}
}

