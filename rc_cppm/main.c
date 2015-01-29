#include <stdio.h>
#include <lpc17xx/lpc17xx.h>
#include <lpc17xx/openlpc.h>
#include <lpc17xx/lpc17xx_ssp.h>

unsigned int radio_data[8] = {0,0,0,0,0,0,0,0};

int servo_steer(float value){
	int servo_set = 113; //Servo centered on Tamiya Chasis
	
	if((value < -1.0) || (value > 1.0)){
		return -1; 	//Don't update and return an error.
	} else {
		//servo_set = (int)(226 * value); //Get an integer value from our turn percentage.
		servo_set = (int)(113 * value);
	} 
	
	//max = 452
	// min = 226
	
	LPC_PWM1->MR1 = 4310 + 339 + servo_set;	//Add to the minimum servo value.
	LPC_PWM1->LER |= (1<<1);			//Load enable Match register 4 (MR4), sets the new value. 
	return 0;	
}


void servo_setup(void){
	LPC_SC->PCONP |= 1<<6; 				//Turn on PWM
	LPC_SC->PCLKSEL0 |= (1<<12)|(1<<13);// PWM pclock/8
	LPC_PINCON->PINSEL4 |= (1<<0); 		//2.0 is PWM 1.1

	LPC_PWM1->PCR |= (1<<9);			//Turn on PWM 1.1 output
	LPC_PWM1->PR = 40;					//Prescale register value to get ~60Hz
	LPC_PWM1->MR0 = 5000;				//Match0 register
	LPC_PWM1->MR1 = 4580;				//Match4 register, opposite because of hardware inverter.
	LPC_PWM1->TCR |= (1<<0)|(1<<3); 	//Enable counter and PWM.
}

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
	LPC_MCPWM->MCPW2 = motor_speed;
	return 0;
}

int motor_brake(float value){
	if(LPC_MCPWM->MCPW2 > 0){
	//	return -1;
	}
	if((value < 0.0) || (value > 1.0)){
		return -1;
	}	
	int motor_brake = 0;
	motor_brake = (int)(20000*value);
	LPC_MCPWM->MCPW2 = 0;
	LPC_MCPWM->MCPW1 = motor_brake;
	return 0;
}

int rgb_set(int r,int g,int b){
	LPC_PWM1->MR2 = g*19.61;
	LPC_PWM1->MR3 = b*19.61;
	LPC_PWM1->MR4 = r*19.61;
	LPC_PWM1->LER |= (1<<4)|(1<<3)|(1<<2);		//Load enable Match register 2/3/4, sets the new value. 
	return 0;	
}

void rgb_setup(void){
	LPC_SC->PCONP |= 1<<6; 						//Turn on PWM
	LPC_SC->PCLKSEL0 |= (1<<12)|(1<<13);		// PWM pclock/8
	LPC_PINCON->PINSEL4 |= (1<<2)|(1<<4)|(1<<6); //2.1/2/3 is PWM 1.2/3/4
	LPC_PWM1->PCR |= (1<<10)|(1<<11)|(1<<12);	//Turn on PWM 1.4 output
	LPC_PWM1->MR4 = 0;							//Set R,G,B to 0,0,0
	LPC_PWM1->MR3 = 0;
	LPC_PWM1->MR2 = 0;
}

void motor_setup(void){
	LPC_SC->PCONP |= 1<<17; 			//Turn on Motor Control PWM
	LPC_SC->PCLKSEL1 |= (1<<30)|(1<<31);// MC pclock/8
	
	LPC_PINCON->PINMODE0 |= (1<<9); //OTW pullup off.
	LPC_PINCON->PINMODE_OD0 |= (1<<4);
	LPC_PINCON->PINMODE1 |= (1<<21); //I_Sense pullup off.
	LPC_PINCON->PINMODE3 |= (1<<25)|(1<<19); //turn off pullups

	LPC_GPIO0->FIODIR |= (1<<6);
	LPC_GPIO0->FIOCLR |= (1<<6);
	
	//1.28 = speed, 1.25 = brake
	
	//MCOA1 = brake
	//MCOA2 = speed
	
	//0.6  = SD
	//0.4 = OTW
	//0.26 = i_sense
	
	LPC_MCPWM->MCPER2 = 20000;
	LPC_MCPWM->MCPW2 = 0;
	LPC_MCPWM->MCPER1 = 20000;
	LPC_MCPWM->MCPW1 = 0;
	
	LPC_MCPWM->MCDEADTIME = (1024)|(1024 << 10);
	
	LPC_MCPWM->MCCON_SET = (1<<16)|(1<<8)|(1<<18)|(1<<10);
	LPC_PINCON->PINSEL3 |= (1<<24)|(1<<18); //P1.19 is MCOA0 and P1.22 is MCOB0,1.25 = MC0A1
	
}

void receiver_setup(void)
{
	
	LPC_SC->PCONP |= (1<<1); //ensure Timer0 is on
	LPC_SC->PCLKSEL1 |= (00<<2); //Timer0 clock
	LPC_PINCON->PINSEL3 |= 0x3 << 20; //Pin1.26 is CAP0.0
	
	LPC_TIM0->IR |= (1<<4);//enable interrupt on capture channel 0 event
	
	LPC_TIM0->TCR |= (1<<1);
	LPC_TIM0->TCR = 0;
	LPC_TIM0->TCR = 1; //Enable counters
	
	LPC_TIM0->CCR |= (1<<1)|(1<<2);//Capture on falling edge and interrupt.
	
	NVIC_EnableIRQ(TIMER0_IRQn);
	
}

void TIMER0_IRQHandler(void)
{

	static int state = 0;
	volatile int capture = LPC_TIM0->CR0; //get the captured timer value
	
	if(state <= 0) //Reset state, wait for a start pulse
	{ 
		if(capture > 50000)
		{
			state = 1;
		}
	}
	
	if(state >= 1 && state <= 8) //Get each of the 8 channels
	{ 
		if (capture > 50000) //Reset state machine if we get a long pulse.
		{
			state = 0;
		}
		
		radio_data[state-1] = capture;
		state++;
	}
	
	if(state > 8) //Wait for another start
	{
		state = 0;
	}
	
	LPC_TIM0->IR |= (1<<4); //Clear the interrupt
	LPC_TIM0->TC = 0;  //Reset the timer counter
	
}

int clip(int input)
{
	
	if(input > 48000){
		input = 48000;
	}
	if(input < 24000){
		input = 24000;
	}
	
	return input;
}

int main()
{
	LPC_GPIO1->FIODIR = 1 << 18 | 1 << 27;
	LPC_GPIO0->FIODIR = 1 << 4 | 1 << 5 | 1 << 6;
	rgb_setup();
	rgb_set(0x00,0xFF,00);

	motor_setup();
	receiver_setup();
	servo_setup();	 //Initialize servo
	servo_steer(0);

	rgb_set(0xFF,0,0);
	
	int throttle = 0;
	int steering = 0;
	int rudder = 0;
	int elevator = 0;

	int speed = 0;
	int servo_position = .5;
	int stop = 0;

	while(1){

		throttle = radio_data[0];
		steering = radio_data[1];
		rudder = radio_data[2];
		elevator = radio_data[3];
		
		
		if(throttle > 48000){
			throttle = 48000;
		}
		if(throttle < 24000){
			stop = 1;
			throttle = 24000;
		} else {
			stop = 0;
		}
		
		steering = clip(steering);
		rudder = clip(rudder);
		elevator = clip(elevator);
		
		speed = (throttle-24000) / 2400;
		servo_position = (steering-24000) / 2400;
		
		printf("1:[");
		int i = 0;
		while(i < 11){
			if(i == speed){
				printf("|");
			} else {
				printf(" ");
			}
			i++;
		}
		printf("] 2:[");
		i = 0;
		while(i < 11){
			if(i == servo_position){
				printf("o");
			} else {
				printf(" ");
			}
			i++;
		}
		printf("] 3:[");
		i = 0;
		while(i < 11){
			if(i == (rudder-24000) / 2400){
				printf("|");
			} else {
				printf(" ");
			}
			i++;
		}
		printf("] 4:[");
		i = 0;
		while(i < 11){
			if(i == (elevator-24000) / 2400){
				printf("|");
			} else {
				printf(" ");
			}
			i++;
		}
		if(radio_data[4] < 32000){
			printf("][v]");
		} else if(radio_data[4] < 40000){
			printf("][-]");
		} else {
			printf("][^]");
		}
		if(radio_data[5] < 32000){
			printf("[v]");
		} else if(radio_data[5] < 40000){
			printf("[-]");
		} else {
			printf("[^]");
		}
		if(radio_data[6] < 32000){
			printf("[v]");
		} else if(radio_data[6] < 40000){
			printf("[-]");
		} else {
			printf("[^]");
		}
		if(radio_data[7] < 32000){
			printf("[v]");
		} else if(radio_data[7] < 40000){
			printf("[-]");
		} else {
			printf("[^]");
		}
		printf("\r\n");
		
		delay_ms(5);
	}
	
	return 0;
}




