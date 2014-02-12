/*command interface
*
*	register read|write address numBytes [data (lsb first) 	--- writes or reads n bytes from address
*	payload	read|write numBytes [data]						--- writes or reads n bytes from the radio
*	flush tx|rx												--- flushes the tx or rx  FIFO
*	reuse													--- reuse last tx payload until a flush tx
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lpc17xx/lpc17xx.h>
#include <lpc17xx/openlpc.h>


#define R_REGISTER(X) (X & 0x1F)
#define W_REGISTER(X) (0x20)|(X & 0x1F)

#define R_RX_PAYLOAD 0x61
#define W_TX_PAYLOAD 0xA0

#define FLUSH_TX 0xE1
#define FLUSH_RX 0xE2

#define REUSE_TX_PL 0xE3
#define ACTIVATE 0x50

#define R_RX_PL_WID 0x60
#define W_ACK_PAYLOAD(X) (0xA8)|(X & 0x7)
#define W_TX_PAYLOAD_NO_ACK 0xB0

#define NOP 0xFF
#define CMD_MAX 512

int spi_move(unsigned int data){
	
	while(LPC_SSP1->SR & (1<<3)){} 	//block if the SSP write FIFO is full.
	LPC_SSP1->DR = (data); 			//write the data
	while(LPC_SSP1->SR & (1<<4)){} 	//block if busy
	return LPC_SSP1->DR;			//read and return the data
	
}

int write_reg(int regAddress,unsigned char *data, int numBytes){
	
	unsigned int count = 0;
	
	LPC_GPIO0->FIOCLR |= (1<<6); //pull CS_N low
	
	(void)spi_move(W_REGISTER(regAddress)); //write register address;
	while(count < numBytes){
		
		(void)spi_move(*(data+count));
		count++;
		
	}
	
	LPC_GPIO0->FIOSET |= (1<<6); //pull CS_N high

	return 0;
}

int write_payload(unsigned char *data, int numBytes){
	
	unsigned int count = 0;
	
	LPC_GPIO0->FIOCLR |= (1<<6); //pull CS_N low
	
	(void)spi_move(W_TX_PAYLOAD); //write register address;
	while(count < numBytes){
		
		(void)spi_move(*(data+count));
		count++;
		
	}
	
	LPC_GPIO0->FIOSET |= (1<<6); //pull CS_N high

	return 0;
}

int read_payload(unsigned char *data, int numBytes){
	
	unsigned int count = 0;
	
	LPC_GPIO0->FIOCLR |= (1<<6); //pull CS_N low
	spi_move(R_RX_PAYLOAD);
	while(count < numBytes){
		
		*(data+count) = spi_move(NOP);
		count++;
		
	}
	
	while(LPC_SSP1->SR & (1<<4)){} //wait for SPI transactions to finish
	LPC_GPIO0->FIOSET |= (1<<6); //pull CS_N high
	
	return 0;
}

int read_reg(int regAddress,unsigned char *data, int numBytes){
	
	unsigned int count = 0;
	
	LPC_GPIO0->FIOCLR |= (1<<6); //pull CS_N low
	spi_move(R_REGISTER(regAddress));
	while(count < numBytes){
		
		*(data+count) = spi_move(NOP);
		count++;
		
	}
	
	while(LPC_SSP1->SR & (1<<4)){} //wait for SPI transactions to finish
	LPC_GPIO0->FIOSET |= (1<<6); //pull CS_N high
	
	return 0;
}

char *remove_newline(char *s) //taken from beej.us fgets page.
{
    int len = strlen(s);

    if (len > 0 && s[len-1] == '\n')  // if there's a newline
        s[len-1] = '\0';          // truncate the string

    return s;
}

int main()
{

	printf("Welcome! Outputs are P4.28 and P4.29\n\r");
	
	LPC_PINCON->PINSEL0 = (1<<15)|(1<<17)|(1<<19);//SCK1,MISO1,MOSI1 to the respective pins,
	
	LPC_SC->PCONP |= (1<<10); //Enable SSP1 power bit

	//LPC_SC->PCLKSEL0 |= 0; //no need to change.
	LPC_SSP1->CPSR |= (1<<3); //Should be 3MHz after prescaler.
	
	LPC_SSP1->CR0 |= 7|(0<<8); //8 bit SPI
	
	
	LPC_GPIO1->FIODIR |= 1 << 18 | 1 << 27; // set led pins to outputs.
	LPC_GPIO1->FIOSET |= 1<<18 | 1 <<27; //turn off LEDs, active low signals.
	LPC_PINCON->PINMODE0 |= (3<<16)|(3<<18); //pull down on MOSI and MISO.
	LPC_PINCON->PINMODE9 |= (1<<25)|(1<<27); //turn off pullups on pins 4.28 and 4.29
	LPC_GPIO4->FIODIR = (1<<28)|(1<<29); //turn on as outputs
	LPC_GPIO0->FIODIR |= (1<<6);
	LPC_GPIO4->FIOCLR |= (1<<28); //turn off initially, just to be in a known state.
	LPC_GPIO4->FIOCLR |= (1<<29);
	

	
	//4.29 is CE which changes between read and write on the tcvr.
	
	LPC_SSP1->CR1 |= (1<<1); //enable SSP1
	

	unsigned char readBuffer[40];
	int readCount = 1;
	int readAddress = 0x0;
	
	int writeCount = 2;
	int writeAddress = 0x0;
	
	char command[CMD_MAX]; //CMD_MAX (512 byte) buffer for UI
	int c;
	int len = 0;
	unsigned char writeBuffer[40];
	int toggle = 0;

	
	while(1){
		
		printf("Enter Command:");
		
		fgets(command, CMD_MAX, stdin);
		while ((c=getchar()) != EOF && c != '\n'); //flush stdin
		remove_newline(command);
		
		
		len = strlen(command);
	  	if(len > 0){
		
	  	 	int cmdCnt = 0; // we are here because there was 1 command.
	  	 	int cmdList[37]; //max data is 32 bytes + 2 byte command, 1 bytes address, and 1 byte data count.
			int cmdLengthList[37];
			
			cmdList[0] = 0;
			cmdLengthList[0] = 0;
	  	 	
			//sweep through the list taking note of where a word starts and stops
	
	  	 	for(c=0;c<len;c++){
	  	 	
				if(command[c] != ' ')
				{
					if((command[c-1] == ' ' || c == 0) && (command[c] >= '0' && command[c] <= 'z')){
						cmdList[cmdCnt] = c; //store the start of a word
					}
					
					if((command[c+1] == ' ' || command[c+1] == '\0') && (command[c] >= '0' && command[c] <= 'z')){
						cmdLengthList[cmdCnt] = c - cmdList[cmdCnt] + 1; //store the legnth of the word
						cmdCnt++;
					}
				}
	
			}
			
			//loop through the list and add null characters for easy string functions, not needed as we have length, but nice for printf.
			
			for(c=0;c<cmdCnt;c++){
				*(command + cmdList[c] + cmdLengthList[c]) = (unsigned int)NULL;
			}
			
			char *endptr;
			
			if(*(command+cmdList[0]) == 'r' && *(command+cmdList[0]+1) == 'e' && *(command+cmdList[0]+2) == 'g'){ //register command
				
				if(*(command+cmdList[1]) == 'r' && cmdCnt == 4){ //read command
					
					readAddress = (unsigned int)strtol(command+cmdList[2],&endptr,0);
					readCount = (unsigned int)strtol(command+cmdList[3],&endptr,0);
					
					read_reg(readAddress, readBuffer, readCount);
					
					int i = 0;
					
					while(i < readCount){
						printf("\n\rRead: %d\n\r", readBuffer[i]);
						i++;
					}
					
					//printf("Read Address: %d\n\rRead Count: %d\n\r", readAddress,readCount);
					
				} else if(*(command+cmdList[1]) == 'w' && cmdCnt <= 36){ //write command
					
					writeAddress = (unsigned int)strtol(command+cmdList[2],&endptr,0);
					writeCount = (unsigned int)strtol(command+cmdList[3],&endptr,0);
					
					if((cmdCnt-4) == writeCount){
						
						for(c=0;c<writeCount;c++){
							
							writeBuffer[c] = (unsigned int)strtol(command+cmdList[4+c],&endptr,0);
							printf("Buffer[%d]:%d\n\r", c, writeBuffer[c]);
							
						}
						
					}
					write_reg(writeAddress, writeBuffer, writeCount);
					printf("Write Address: %d\n\rWrite Count: %d\n\rCmd Cnt: %d\n\r", writeAddress, writeCount, cmdCnt-4);
				}
				
			/*PAYLOAD */
				
			} else if(*(command+cmdList[0]) == 'p') {
				
				if(*(command+cmdList[1]) == 'r' && cmdCnt == 3){ //read command
					
					readCount = (unsigned int)strtol(command+cmdList[2],&endptr,0);
					if(readCount > 32){
						readCount = 32;
					}
					read_payload(readBuffer, readCount);
					
					int i = 0;
					
					while(i < readCount){
						printf("\n\rRead: %d", readBuffer[i]);
						i++;
					}
					
					printf("Payload read count: %d\n\r", readCount);
					
				} else if(*(command+cmdList[1]) == 'w' && cmdCnt <= 36){ //write command
					
					writeCount = (unsigned int)strtol(command+cmdList[2],&endptr,0);
					
					if((cmdCnt-3) == writeCount){
						
						for(c=0;c<writeCount;c++){
							
							writeBuffer[c] = (unsigned int)strtol(command+cmdList[3+c],&endptr,0);
							printf("Buffer[%d]:%d\n\r", c, writeBuffer[c]);
							
						}
						
					}
					write_payload(writeBuffer,writeCount);
					printf("Write Count: %d\n\rCmd Cnt: %d\n\r", writeCount, cmdCnt-3);
				}
				
				
				
					
			} else if(*(command+cmdList[0]) == 'r' && *(command+cmdList[0]+1) == 'e' && *(command+cmdList[0]+2) == 'u' && cmdCnt == 1){
					(void)spi_move(REUSE_TX_PL);
					printf("reuse active\n\r");

			} else if(*(command+cmdList[0]) == 'f' && cmdCnt == 2) {

					if(*(command+cmdList[1]) == 'r'){
						(void)spi_move(FLUSH_RX);
						printf("flush rx\n\r");
					}	

					if(*(command+cmdList[1]) == 't'){
						(void)spi_move(FLUSH_TX);
						printf("flush tx\n\r");
					}
			} else if(*(command+cmdList[0]) == 'i' && cmdCnt == 1) {
				if(toggle == 0){
					LPC_GPIO4->FIOCLR |= (1<<28);
					LPC_GPIO4->FIOCLR |= (1<<29);
					printf("4.29 and 4.28 off\n\r");
					toggle = 1;
				} else {
					LPC_GPIO4->FIOSET |= (1<<28); //turn off initially, just to be in a known state.
					LPC_GPIO4->FIOSET |= (1<<29);
					printf("4.29 and 4.28 ON\n\r");
					toggle = 0;
				}
				
			} else {
					printf("Error\n\r");
			}		
	
		}
	}

	return 0; //what are you doing here?
}

