#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "motor.h"

int main(void)
{
	Motor_Init();
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
	Motor_Move(-1,64000,4000,4000,4000);
	while(1);
}
