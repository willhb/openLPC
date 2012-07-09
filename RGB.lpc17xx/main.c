#include <stdio.h>
#include <stdlib.h>
#include <lpc17xx/lpc17xx.h>
#include <lpc17xx/openlpc.h>

/*		LPC1769 RGB Demo
*		William Breidenthal 3/2012
*
*		Hardware PWM RGB demo. Connect RGB LED to pins 2.1,2.2, and 2.3. 
*		Open serial consol, accepts range 00 to FF seperated by commas.
*		Examples:
*		FF,FF,FF or 0,0,FF or 25,25,FA
*
*		Pressing return will print out the PWM value in decimal and update the LED.
*
*		Example uses 60Hz output to be compatible with servos on the other PWM output.
*		Demo expects valid input, otherwise it will crash. Data validation is ommited 
*		to maintain simplicity.
*/

int rgb_set(int r,int g,int b){
	LPC_PWM1->MR2 = g;
	LPC_PWM1->MR3 = b;
	LPC_PWM1->MR4 = r;
	LPC_PWM1->LER |= (1<<4)|(1<<3)|(1<<2);		//Load enable Match register 2/3/4, sets the new value. 
	return 0;	
}

void rgb_setup(void){
	LPC_SC->PCONP |= 1<<6; 						//Turn on PWM
	LPC_SC->PCLKSEL0 |= (1<<12)|(1<<13);		// PWM pclock/8
	LPC_PINCON->PINSEL4 |= (1<<2)|(1<<4)|(1<<6); //2.1/2/3 is PWM 1.2/3/4

	LPC_PWM1->PCR |= (1<<10)|(1<<11)|(1<<12);	//Turn on PWM 1.4 output
	LPC_PWM1->PR = 40;							//Prescale register value to get ~60Hz
	LPC_PWM1->MR0 = 5000;						//Match0 register
	LPC_PWM1->MR4 = 0;							//Set R,G,B to 0,0,0
	LPC_PWM1->MR3 = 0;
	LPC_PWM1->MR2 = 0;
	LPC_PWM1->TCR |= (1<<0)|(1<<3); 			//Enable counter and PWM.
}

int main()
{
	rgb_setup();
	
	int r = 0;
	int g = 0;
	int b = 0;
	
	while(1){
		scanf("%x,%x,%x", &r,&g,&b); //Get input in hex.
		
		r *= 19.61;	//5000/255 ~= 19.61
		g *= 19.61;
		b *= 19.61;
		
		rgb_set(r,g,b); //Update LEDs.
		
		printf("R: %d, G: %d, B: %d \n\r", r,g,b); //Print back the results.
	}

	return 0;
}

