#ifndef __USART_H
#define __USART_H

struct Motormove_Data {
	signed int dirtemp1;
	unsigned int step1;
	unsigned int accel1;
	unsigned int decel1;
	unsigned int speed1;
}; //电机运动参数中间变量结构体

extern struct Motormove_Data data;
extern int serial_status;
void USART1_Init(u32 bound);

#endif
