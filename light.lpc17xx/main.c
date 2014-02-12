#include <stdio.h>
#include <lpc17xx/lpc17xx.h>
#include <lpc17xx/openlpc.h>
#include <stdlib.h>

#define ON servo_steer(1)
#define OFF servo_steer(0)

/*		LPC1769 Servo Demo
*		William Breidenthal 3/2012
*
*		This is a servo demo that has been customized to use the HiTec HS-9605MG servo with
*		the custom servo power/buffer board. If a different board or servo combo is used it
*		will be necessary to make adjustments. The current resolution exceeds the servo's,
*		it is not necessary to increase it any more.												
*
*		servo_setup() will setup a 58Hz PWM output on pin 2.3 (PWM1.4) with the servo centered.
*		servo_steer() takes a float in the range 0.0->1.0 and returns 0 with success, and -1 on failure. 
*/

int servo_steer(int value){
	
	if(value == 1){
	LPC_PWM1->MR1 = 390;
	} else {
		LPC_PWM1->MR1 = 0;
	}	//Add to the minimum servo value.
	LPC_PWM1->LER |= (1<<1);			//Load enable Match register 4 (MR4), sets the new value. 
	return 0;	
}

void servo_setup(void){
	LPC_SC->PCONP |= 1<<6; 				//Turn on PWM
	LPC_SC->PCLKSEL0 |= (1<<12)|(0<<13);// PWM pclock/8
	LPC_PINCON->PINSEL4 |= (1<<0); 		//2.0 is PWM 1.1

	LPC_PWM1->PCR |= (1<<9);			//Turn on PWM 1.1 output
	LPC_PWM1->PR |= 2;					//Prescale register value to get ~60Hz
	LPC_PWM1->MR0 |= 795;				//Match0 register
	LPC_PWM1->MR1 |= 1;				//Match4 register, opposite because of hardware inverter.
	LPC_PWM1->TCR |= (1<<0)|(1<<3); 	//Enable counter and PWM.
}

//#define ON LPC_PWM1->TCR &= ~(1<<1)
//#define OFF LPC_PWM1->TCR |= (1<<1)



int main()
{
	// initialize the LED pin as an output
	LPC_GPIO1->FIODIR = 1 << 18 | 1 << 27;
	LPC_GPIO1->FIOSET = 1 << 18 | 1 << 27;
	LPC_GPIO0->FIODIR = 1 << 4 | 1 << 5 | 1 << 6;
	
	servo_setup();	 //Initialize servo

	servo_steer(0.0);

	OFF;

	//21ms on
	//6.5ms off
	//2.5ms on
	//6.5ms off
	//2.5ms on
	//9 ms off
	//2.5 on
	//6.5 off
	//2.5 on
	//2.5 off
	//7 on
	//16ms off
	
	int i = 0;
	int c = 0;

	while(1){
		printf("hello");
		c = getchar();
		
		if(c == 'o'){
		ON;
		delay_ms(20);
		OFF;
		delay_ms(7);
		ON;
		delay_ms(2.6);
		OFF;
		delay_ms(7);
		ON;
		delay_ms(2.6);
		OFF;
		delay_ms(9);
		ON;
		delay_ms(2.6);
		OFF;
		delay_ms(6);
		ON;
		delay_ms(2.5);
		OFF;
		delay_ms(2.2);
		ON;
		delay_ms(7);
		OFF;
		delay_ms(16);
		if(i == 100){
			delay_ms(500);
		}
		if(i >= 200){
			delay_ms(100);
			i = 0;
			c = 0;
		} else {
			i++;
		}
	}
	}

	return 0;
}

