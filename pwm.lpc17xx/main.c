#include <stdio.h>
#include <lpc17xx/lpc17xx.h>
#include <lpc17xx/openlpc.h>

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

int servo_steer(float value){
	int servo_set = 113; //Servo centered on Tamiya Chasis
	
	if((value < 0.0) || (value > 1.0)){
		return -1; 	//Don't update and return an error.
	} else {
		servo_set = (int)(226 * value); //Get an integer value from our turn percentage.
	} 
	
	//max = 452
	// min = 226
	
	LPC_PWM1->MR1 = 4310 + 226 + servo_set;	//Add to the minimum servo value.
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

int main()
{
	// initialize the LED pin as an output
	LPC_GPIO1->FIODIR = 1 << 18 | 1 << 27;
	LPC_GPIO0->FIODIR = 1 << 4 | 1 << 5 | 1 << 6;
	
	servo_setup();	 //Initialize servo
	servo_steer(.5); //Steer to center	

	/*LPC1769 Servo Demo*/

	int polarity = 0;
	float a = 0;
	
	
	int r = 0;
	
	float sset = 0.0;

	while(1){
		scanf("%d", &r); //Get input in hex.
		
		//r *= 19.61;	//5000/255 ~= 19.61
		sset = r/255.0;
		servo_steer(sset);
		
		printf("R: %d : %f\n\r", r,sset); //Print back the results.
	}

	while(1){
		if(polarity == 0){
			a += .001 ;
		}
		if(polarity == 1){
			a -= .001;
		}
		if(a > 1.0){
			polarity = 1;
			delay_ms(100);
		}
		if(a < 0.0){
			polarity = 0;
			delay_ms(100);
		}
		servo_steer(a);
		delay_ms(2);
	}
	

	
	while(1){
		servo_steer(0.0);
		delay_ms(500);
		servo_steer(.333);
		delay_ms(500);
	}
	


	return 0;
}

