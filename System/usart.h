#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"

#define   DATA_SIZE     256//�������鳤��
#define 	SIZE          512//�������鳤��

extern char     ReceiveData[DATA_SIZE];	//�����������
extern uint8_t  SendBuffer[SIZE];//���巢������

struct Motormove_Data {
	signed int dirtemp1;
	unsigned int step1;
	unsigned int accel1;
	unsigned int decel1;
	unsigned int speed1;
}; //����˶������м�����ṹ��

extern struct Motormove_Data data;
extern int serial_status;
extern int temp;
extern unsigned int flag;
void USART1_DMA_Config(void);
void USART1_Init(u32 bound);
void Usart_Printf(const char * format, ...);
	
#endif
