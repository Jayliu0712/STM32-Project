#ifndef __MOTOR_H
#define __MOTOR_H
#include "stm32f10x.h"

#define CW 0
#define CCW 1

#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3 //״̬�궨�壬�������

#define TIM1_FREQ 1000000
#define SPR 200
#define DIV_SPR (SPR*16)
#define ALPHA ((float)(2*3.14159/DIV_SPR)) //���庯���������������

#define A ((long)(ALPHA*TIM1_FREQ*100))
#define B ((long)(TIM1_FREQ*0.424))
#define C ((float)(ALPHA*2))
#define D ((float)(ALPHA*200))

extern int ARR_Val;						
typedef struct {
  unsigned char run_state :3;//�������״̬
  unsigned char dir : 1;//������з���
  unsigned int step_delay;//��һ��������ʱ���ڣ�����ʱΪ���ٶ�����
  unsigned int start_decel;//��ʼ���ٵ�λ��  
  signed int decel_val;//���پ���  
  signed int min_delay;//��С��ʱ��������ٶȣ�  
  signed int accel_count;//���ٻ��߼��ټ�����
} speedRampData;

#define DIR(a)	if (a == CW)\
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);\
else GPIO_SetBits(GPIOB,GPIO_Pin_3) 
//define�����һ��������һ�� ����βӦ�÷�б�ߡ�\������

void Motor_Move(signed int dirtemp, signed int step, unsigned int accel, unsigned int decel, unsigned int speed);
void Motor_Init(void);

#endif
