#include <stdio.h>
#include <stdlib.h>
#include <lpc17xx/lpc17xx.h>
#include <lpc17xx/openlpc.h>

/*		LPC1769 Toggle
*		A simple program that sets a few pins to 0 or to 1. USEFUL!
*/



int main()
{
	//spew a bunch of garbage to tell someone what to do (or if i forget).
	printf("Welcome! Outputs are P4.28 and P4.29\n\r");
	printf("Enter the appropriate number: \n\r 0: 4.28 = 0, 4.29 = 0\n\r 1: 4.28 = 1, 4.29 = 0\n\r 2: 4.28 = 0, 4.29 = 1\n\r 3: 4.28 = 1, 4.29 = 1.\n\r\n\r");
	printf("Enter \"z\" for high impedance, incorrect input remains constant.\n\r");

	LPC_GPIO1->FIODIR = 1 << 18 | 1 << 27; // set led pins to outputs.
	LPC_GPIO1->FIOSET |= 1<<18 | 1 <<27; //turn off LEDs, active low signals.

	LPC_PINCON->PINMODE9 = (1<<25)|(1<<27); //turn off pullups on pins 4.28 and 4.29
	LPC_GPIO4->FIODIR = (1<<28)|(1<<29); //turn on as outputs
	LPC_GPIO4->FIOCLR |= (1<<28); //turn off initially, just to be in a known state.
	LPC_GPIO4->FIOCLR |= (1<<29);
	
	int output = 0; 
	
	while(1){
		printf("\rInput: ");
		output = getchar();
		while ( getchar() != '\n' ); //flush the buffer
		if((output == 48) | (output == 49 )| (output == 50) | (output == 51)){ //make sure we are seeing characters 0 through 3
			
		LPC_GPIO4->FIODIR = (1<<28)|(1<<29);//Set to low impedance outputs!
		
		int p28 = output & 1; //get bitwise values
		int p29 = output>>1 & 1;
		
		if(p28 == 1){ //push the value to the pin, set the LED for good visuals!
			LPC_GPIO4->FIOSET |= (1<<28);
			LPC_GPIO1->FIOCLR |= (1<<18);
		} else {
			LPC_GPIO4->FIOCLR |= (1<<28);
			LPC_GPIO1->FIOSET |= (1<<18);
		}
		
		if(p29 == 1){
			LPC_GPIO4->FIOSET |= (1<<29);
			LPC_GPIO1->FIOCLR |= (1<<27);
		} else {
			LPC_GPIO4->FIOCLR |= (1<<29);
			LPC_GPIO1->FIOSET |= (1<<27);
		}
		
		output = 0;
		printf("\n\rp28: %d, p29: %d \n\r", p28, p29); //Print back the results.
		
		} else if (output == 10) { //just incase the flush doesn't work, ignore the output.
		} else if (output == 'z') {
			LPC_GPIO4->FIODIR = 0 ;//Set to high impedance outputs!
			printf("\n\rHigh impedance mode...\n\r");
		}	else {
			printf("\n\rError, please enter again:\n\r"); //4 buttons, is it that hard?
		}
		delay_ms(200); //delay, because delay.
	}

	return 0; //what are you doing here?
}

