#include <stdio.h>
#include <lpc17xx/lpc17xx.h>
#include <lpc17xx/openlpc.h>
#include <cr_dsplib.h>
#include <math.h>


static short *fft_input  = (short *)0x2007C000; // AHB SRAM0  //See NXP UM10360 Page 12
static short *fft_out = (short *)0x20080000;	 // AHB SRAM1

int main(void)
{
	
	LPC_SC->PCONP |= (1<<12); 				//Turn on ADC power and clock
	LPC_SC->PCLKSEL0 |= (1<<24)|(1<<25);	//Scale the ADC clock by 8 (96/8 = 12MHz)
	
	LPC_PINCON->PINSEL1 |= (1<<14); //P0.23 is connected to AD0.0
	LPC_PINCON->PINMODE1 |= (1<<15); //No pullups/downs
	LPC_ADC->ADCR = (1<<21)|(0<<8); //Turn on ADC
	
	while(1){
		
	volatile short int i;
	volatile short int magnitude = 0;

	for(i = 0; i < 1024; i++)
	{
		LPC_ADC->ADCR |= (1<<24);//start a conversion
		while((LPC_ADC->ADDR0 >> 31) && 1){} //wait for conversion
		fft_input[2 * i] = (LPC_ADC->ADDR0>>4) & 0xFFF; //real input is ADC conversion
		fft_input[(2 * i) + 1] = 0; //real part is 0
	}
	
	vF_dspl_fftR4b16N1024(fft_out, fft_input); //compute the FFT

	printf("\n\r-------------------------\n\r"); //print a line
	
	for(i = 1; i < 74; i+=1) //Display the first 64 values, don't include DC.
	{
		magnitude = (int)sqrt((pow(fft_out[2 * i], 2) + pow(fft_out[(2 * i) + 1], 2))); // m = sqrt(a^2 + b^2)
		
		printf("%d Hz: ", 332*i); //Without using interrupts the sampling frequency is a weird number.
		
		int mag = 0;
		
		while(mag < magnitude/3){ //Print characters scaled to the magnitude of the frequency.
			printf("*");
			mag++;
		}
		printf("\n\r");

	}
	
	
		delay_ms(50); //Slow down the update rate, otherwise the console can't display fast enough.
	}
}
