#include <stdio.h>
#include <lpc17xx/lpc17xx.h>
#include <lpc17xx/openlpc.h>

/*		LPC1769 RTC demo
*		William Breidenthal 5/22/2012
*/

void RTC_IRQHandler(void)
{
	//Clear interrupt flags for both clock and alarms.
	LPC_RTC->ILR |= (1<<0)|(1<<1);
	//Toggle the red LED on the OpenLPC board.
	LPC_GPIO1->FIOPIN ^= (1<<18);
	//Print the current time. 
	printf("H:%d M:%d S:%d \n\r", (LPC_RTC->CTIME0 & 0x1F0000)>>16,(LPC_RTC->CTIME0 & 0x3F00)>>8,(LPC_RTC->CTIME0 & 0x3F));
}


int main()
{
	// initialize the LED pin as an output
	LPC_GPIO1->FIODIR = 1 << 18 | 1 << 27;
	
	printf("Welcome to the OpenLPC1769 RTC Demo\n\r\n\r");
	
	//Make sure the RTC block has power
	LPC_SC->PCONP |= (1<<9);
	
	//Enable the clock
	LPC_RTC->CCR = 1;
	
	//Enable interrupts every 1 second
	LPC_RTC->CIIR |= (1<<0);
	
	//Remove the oscillator fail flag set on reset.
	LPC_RTC->RTC_AUX = (1<<4);
	
	//Print out the content of GPREG0-4, as long as the RTC has power this should stay the same through reset.
	printf("GPREG0 Value: %d\n\r", LPC_RTC->GPREG0);
	printf("GPREG1 Value: %d\n\r", LPC_RTC->GPREG1);
	printf("GPREG2 Value: %d\n\r", LPC_RTC->GPREG2);
	printf("GPREG3 Value: %d\n\r", LPC_RTC->GPREG3);
	printf("GPREG4 Value: %d\n\r\n\r", LPC_RTC->GPREG4);
	
	//Print out the oscillator fail flag, if it is 1 then there is a hardware issue.
	printf("Oscillator Status (0=OK;1=Failed): %d\n\r", (LPC_RTC->RTC_AUX && 1<<4));
	printf("Interrupts enabled. Entering idle... \n\r");

	//Enable the RTC interrupt.
	NVIC_EnableIRQ(RTC_IRQn);
	

	//Do nothing and wait for an interrupt.
	while(1){}

	return 0;
}

