#include <stdio.h>
#include <stdlib.h>
#include <lpc17xx/lpc17xx.h>
#include <lpc17xx/openlpc.h>

/*		LPC1769 i2c Demo
*		William Breidenthal 4/2012
*/

#define TEMP_R 			0x91
#define TEMP_W 			0x90

#define P_TEMP	 	0x00;
#define point_conifg 	0x01;
#define point_tlow 		0x02;
#define point_thigh	 	0x03;

	int counter = 0;
	int state = 0;
	int address = TEMP_W;
	int data = P_TEMP;

int main()
{
	
	LPC_SC->PCONP |= (1<<19); //Make sure I2c1 is on
	LPC_SC->PCLKSEL1 |= (1<<6)|(1<<7); //I2C clock /8
	
	LPC_PINCON->PINMODE0 |= (1<<1)|(1<<3); //Turn off pull-ups and pull downs.
	LPC_PINCON->PINMODE_OD0 |= (1<<0)|(1<<1); //P0.0 and P0.1 are Open Drain
	LPC_PINCON->PINSEL0 |= (1<<0)|(1<<1)|(1<<2)|(1<<3); //P0.0 and P0.1 are SDA1 and SCL1
	
	delay_ms(1000);
	
	NVIC_EnableIRQ(I2C1_IRQn);
	
	LPC_I2C1->I2SCLL = 60; //Set SCL Low and High duty cycle
	LPC_I2C1->I2SCLH = 60;
	
	LPC_I2C1->I2CONSET = (1<<6); //Enable I2C

	address = 0x90;
	data = 0x00;
	LPC_I2C1->I2CONSET = (1<<5); 
	
	while(1){
		delay_ms(1000);
		if(counter == 1){
			counter = 0;
			LPC_I2C1->I2CONCLR = (1<<4); 
			LPC_I2C1->I2CONSET = (1<<5); 
		}

	}

	return 0;
}

void I2C1_IRQHandler(void)
{
	int I2C_Status = LPC_I2C1->I2STAT;
	printf("Status: 0x%x", I2C_Status);
	
	switch(I2C_Status){
		case 0x8:
			LPC_I2C1->I2DAT = 0x91;
			LPC_I2C1->I2CONSET = 0x04;
			LPC_I2C1->I2CONCLR = (1<<3)|(1<<5);	
			printf(": Done \n\r");
			break;
		case 0x18:
			LPC_I2C1->I2DAT = 0x00;
			LPC_I2C1->I2CONSET = 0x14;
			LPC_I2C1->I2CONCLR = 0x08;
			printf(":Wrote: %x Ack Done \n\r", LPC_I2C1->I2DAT);
			state = 1;
			break;
		case 0x20:
			LPC_I2C1->I2CONSET = 0x14;
			LPC_I2C1->I2CONCLR = 0x08;
			printf(": Done \n\r");
			break;
		case 0x10:
			LPC_I2C1->I2DAT = 0x90;
			LPC_I2C1->I2CONSET = 0x04;
			LPC_I2C1->I2CONCLR = 0x08;
			LPC_I2C1->I2CONCLR = (1<<3);
			printf(": Done \n\r");
			break;
		case 0x48:
			LPC_I2C1->I2CONSET = 0x04;
			LPC_I2C1->I2CONCLR = 0x08;
			printf(": Done \n\r");
			break;
		case 0x38:
			LPC_I2C1->I2CONSET = 0x24;
			LPC_I2C1->I2CONCLR = 0x08;
			printf(": Done \n\r");
			break;
		case 0x40:
			LPC_I2C1->I2CONSET = (1<<2);
			LPC_I2C1->I2CONCLR = 0x08;
			printf(": Done \n\r");
			break;
		case 0x50:
			if(state == 0){
				LPC_I2C1->I2CONSET = 0x04;
				LPC_I2C1->I2CONCLR = 0x08;
				printf(":Read: %d  Done \n\r", (LPC_I2C1->I2DAT<<4)/16);
				state = 1;
			} else {
				LPC_I2C1->I2CONCLR = 0x0C;
				printf(":Read: %d  Done \n\r", (LPC_I2C1->I2DAT)*5/128);
				state = 0;
			}
		break;
		case 0x58:
			printf(":Read: %x  Done \n\r", LPC_I2C1->I2DAT);	
			LPC_I2C1->I2CONSET = 0x14;
			LPC_I2C1->I2CONCLR = 0x08;
			counter = 1;	
			break;
		default:
			printf("Something went wrong...");
			break;
		printf("\n\r");	
	}
	
	delay_ms(10);

}

