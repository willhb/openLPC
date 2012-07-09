#include <stdio.h>
#include <lpc17xx/lpc17xx.h>
#include <lpc17xx/openlpc.h>

/*		LPC1769 Servo Demo
*		William Breidenthal 3/2012
*
*/

//Yay notes: MCOA0 P1.19
//			MCOB0	P1.22
			
//17xx.h weirdness..
//
//MCTIM0 = MCTC0
//MCPER0 = MCLIM0
//MCPW0 = MCMAT0
//MCDEADTIME = MCDT

int motor_speed(float value){
	if(LPC_MCPWM->MCPW1 > 0){
	//	return -1;
	}
	if((value < 0.0) || (value > 1.0)){
		return -1;
	}	
	int motor_speed = 0;
	motor_speed = (int)(20000*value);
	LPC_MCPWM->MCPW1 = 0;
	LPC_MCPWM->MCPW0 = motor_speed;
	return 0;
}

int motor_brake(float value){
	if(LPC_MCPWM->MCPW0 > 0){
	//	return -1;
	}
	if((value < 0.0) || (value > 1.0)){
		return -1;
	}	
	int motor_brake = 0;
	motor_brake = (int)(20000*value);
	LPC_MCPWM->MCPW0 = 0;
	LPC_MCPWM->MCPW1 = motor_brake;
	return 0;
}


void motor_setup(void){
	LPC_SC->PCONP |= 1<<17; 			//Turn on Motor Control PWM
	LPC_SC->PCLKSEL1 |= (1<<30)|(1<<31);// MC pclock/8
	
	LPC_PINCON->PINMODE3 |= (1<<7)|(1<<19); //turn off pullups
	
	LPC_MCPWM->MCPER0 = 20000;
	LPC_MCPWM->MCPW0 = 0;
	LPC_MCPWM->MCPER1 = 20000;
	LPC_MCPWM->MCPW1 = 0;
	
	LPC_MCPWM->MCDEADTIME = (1024)|(1024 << 10);
	
	LPC_MCPWM->MCCON_SET = (1<<0)|(1<<8)|(1<<2)|(1<<10);
	LPC_PINCON->PINSEL3 |= (1<<6)|(1<<18); //P1.19 is MCOA0 and P1.22 is MCOB0,1.25 = MC0A1
	
}

int main()
{
	// initialize the LED pin as an output
	LPC_GPIO1->FIODIR = 1 << 18 | 1 << 27;
	LPC_GPIO0->FIODIR = 1 << 4 | 1 << 5 | 1 << 6;

	motor_setup();
	delay_ms(1000);
	int a = 0;

	while(1){
		if(a == 20000){a = 0;}
			a += 10;
			motor_brake(.5);
		//	delay_ms(100);
			motor_speed(.5);

	}

	return 0;
}

