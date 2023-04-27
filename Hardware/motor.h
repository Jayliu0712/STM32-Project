#ifndef __MOTOR_H
#define __MOTOR_H
#include "stm32f10x.h"

#define CW 0
#define CCW 1

#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3 //状态宏定义，方便调用

#define TIM1_FREQ 1000000
#define SPR 200
#define DIV_SPR (SPR*16)
#define ALPHA ((float)(2*3.14159/DIV_SPR)) //定义函数常量，方便计算

#define A ((long)(ALPHA*TIM1_FREQ*100))
#define B ((long)(TIM1_FREQ*0.424))
#define C ((float)(ALPHA*2))
#define D ((float)(ALPHA*200))

extern int ARR_Val;						
typedef struct {
  unsigned char run_state :3;//电机运行状态
  unsigned char dir : 1;//电机运行方向
  unsigned int step_delay;//下一个脉冲延时周期，启动时为加速度速率
  unsigned int start_decel;//开始减速的位置  
  signed int decel_val;//减速距离  
  signed int min_delay;//最小延时（即最大速度）  
  signed int accel_count;//加速或者减速计数器
} speedRampData;

#define DIR(a)	if (a == CW)\
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);\
else GPIO_SetBits(GPIOB,GPIO_Pin_3) 
//define中如果一个串长于一行 在行尾应用反斜线“\”续行

void Motor_Move(signed int dirtemp, signed int step, unsigned int accel, unsigned int decel, unsigned int speed);
void Motor_Init(void);

#endif
