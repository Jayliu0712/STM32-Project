#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"

#define   DATA_SIZE     256//接收数组长度
#define 	SIZE          512//发送数组长度

extern char     ReceiveData[DATA_SIZE];	//定义接收数组
extern uint8_t  SendBuffer[SIZE];//定义发送数组

struct Motormove_Data {
	signed int dirtemp1;
	unsigned int step1;
	unsigned int accel1;
	unsigned int decel1;
	unsigned int speed1;
}; //电机运动参数中间变量结构体

extern struct Motormove_Data data;
extern int serial_status;
extern int temp;
extern unsigned int flag;
void USART1_DMA_Config(void);
void USART1_Init(u32 bound);
void Usart_Printf(const char * format, ...);
	
#endif
