#include <stdio.h>
#include <lpc17xx/lpc17xx.h>
#include <lpc17xx/openlpc.h>
#include <lpc17xx/lpc17xx_ssp.h>
 
int main()
{
	printf("Welcome to OpenLPC!\n\r");
	printf("%d \n\r",LPC_SC->CCLKCFG);
	delay_ms(1000);
	
	LPC_SC->PCONP |= (1<<21);

	LPC_SC->PCLKSEL1 |= (1<<10);
	LPC_PINCON->PINSEL3 = (3 << 10)|(3 << 8)|(3 << 14);
	LPC_PINCON->PINMODE3 = (3<<8)|(3<<14)|(3<<16)|(3<<10);
	LPC_GPIO1->FIODIR = 1 << 18 | 1 << 27;
	
	LPC_SSP0->CR0 |= 0xF;
	LPC_SSP0->CR1 |= (1<<1);
	LPC_SSP0->CPSR = 4;
	
	int waveform[200];
	
	int average = 0;
	
	while(1){
		
	int timerx =  millis();
	
	if(average == 0){
		for(int i = 0; i < 50 ; i++) {
			LPC_SSP0->DR = 0;
			while ( (LPC_SSP0->SR & ((SSP_SR_BSY)|SSP_SR_RNE)) != SSP_SR_RNE );
			average = 2000;
			average = ((LPC_SSP0->DR >> 2) + average)/2;			
		}
		printf("Average: %d \n\r", average);
	}
	
	for(int i = 0; i < 50 ; i++) {
		LPC_SSP0->DR = 0;
		while ( (LPC_SSP0->SR & ((SSP_SR_BSY)|SSP_SR_RNE)) != SSP_SR_RNE );
		waveform[i] = (LPC_SSP0->DR >> 2);			
	}
	
	int peak = 0;
	
	for(int i = 0; i < 50 ; i++) {
		printf("%d \n\r", waveform[i] - average);
		if(waveform[i] > peak){
			peak = waveform[i];
		}
	}
	
	
	//printf("Waveform Capture Done.\n\r");
	printf("Peak: %d \n\r", peak);
	int bar = (peak - average) / 20;
	while(bar > 0){
		printf("#");
		bar--;
	}
	printf("\n\r");
	int a;
	int b = 0;
	//printf("%d \n\r", millis() - timerx);
	
	while(!b){
		LPC_GPIO1->FIOSET = 1 << 18;
		a = getchar();
		delay_ms(10);
		//a = 'l';
		
		if(a == 'l')
			b =1;
		if(a == 'r'){
			average = 0;
			b = 1;
		}
		LPC_GPIO1->FIOCLR = 1 << 18;	
	};
	
	}

	return 0;
}

