/* William Breidenthal 10/31/2012
*  i2s demo file for my 7400 series contest entry. 
*/

#include <stdio.h>
#include <lpc17xx/lpc17xx.h>
#include <lpc17xx/openlpc.h>
 
char sine[256] = {128,
131,
134,
137,
140,
143,
146,
149,
152,
156,
159,
162,
165,
168,
171,
174,
176,
179,
182,
185,
188,
191,
193,
196,
199,
201,
204,
206,
209,
211,
213,
216,
218,
220,
222,
224,
226,
228,
230,
232,
234,
235,
237,
239,
240,
242,
243,
244,
246,
247,
248,
249,
250,
251,
251,
252,
253,
253,
254,
254,
254,
255,
255,
255,
255,
255,
255,
255,
254,
254,
253,
253,
252,
252,
251,
250,
249,
248,
247,
246,
245,
244,
242,
241,
239,
238,
236,
235,
233,
231,
229,
227,
225,
223,
221,
219,
217,
215,
212,
210,
207,
205,
202,
200,
197,
195,
192,
189,
186,
184,
181,
178,
175,
172,
169,
166,
163,
160,
157,
154,
151,
148,
145,
142,
138,
135,
132,
129,
126,
123,
120,
117,
113,
110,
107,
104,
101,
98,
95,
92,
89,
86,
83,
80,
77,
74,
71,
69,
66,
63,
60,
58,
55,
53,
50,
48,
45,
43,
40,
38,
36,
34,
32,
30,
28,
26,
24,
22,
20,
19,
17,
16,
14,
13,
11,
10,
9,
8,
7,
6,
5,
4,
3,
3,
2,
2,
1,
1,
0,
0,
0,
0,
0,
0,
0,
1,
1,
1,
2,
2,
3,
4,
4,
5,
6,
7,
8,
9,
11,
12,
13,
15,
16,
18,
20,
21,
23,
25,
27,
29,
31,
33,
35,
37,
39,
42,
44,
46,
49,
51,
54,
56,
59,
62,
64,
67,
70,
73,
76,
79,
81,
84,
87,
90,
93,
96,
99,
103,
106,
109,
112,
115,
118,
121,
124};



int main()
{
	printf("7400 Series Contest Entry!\n\r");
	printf("%d \n\r",LPC_SC->CCLKCFG);
	
	LPC_SC->PCONP |= (1<<27); //Enable i2s controller

	LPC_SC->PCLKSEL1 |= (3<<22); //i2s clock = mclk/8 = 12MHz
	
	LPC_PINCON->PINSEL0 |= (1<<14)|(1<<16)|(1<<18);	// Set 0.7/8/9 to I2S TX CLK/WS/SDA
	LPC_PINCON->PINSEL9 |= (1<<26); //Set 4.29 to I2S MCLK
	LPC_PINCON->PINMODE0 |= (1<<14)|(1<<16)|(1<<18); //Disable pullups on 0.7/8/9
	LPC_PINCON->PINMODE9 |= (1<<26); /Disable pullups
	
	LPC_GPIO1->FIODIR = 1 << 18 | 1 << 27;			//Turn on status LEDs
	
	LPC_I2S->I2SDAO = 0;
	LPC_I2S->I2SDAO |= (1<<2)|(7<<6); // I2S mode 8 bit
	
	LPC_I2S->I2STXRATE |= (2)|(1<<8);	//Clock divider to get 12kHz sample rate
	
	LPC_I2S->I2STXBITRATE = 31;		//Transmit rate divider.
	
	LPC_I2S->I2STXMODE = (1<<3);	//Enable the master clock output to use for PWM
	
	LPC_I2S->I2STXFIFO = 0xAA;		//Test signal
	
	int index = 0;
	int a = millis();
	int count = 17;
	int mark = 0;
	
	
	while(1){ 
		
		if((LPC_I2S->I2SSTATE>>16)<=4){ //Check to see if the FIFO is less than half full.
		
		LPC_I2S->I2STXFIFO = sine[index]|(sine[index+1]<<8)|(sine[index+2]<<16)|(sine[index+3]<<24); //Fill up one FIFO slot.
		index += count;
		if(index >= 256){
			index = 0;
		}
		
		//Sweep the frequency range by passing through the sine LUT faster or slower.
		
		if((millis()-a) > 100){ //If it has been 100ms then incrament the frequency.
			a = millis();
			if(mark == 0){
			count += 3;
			} else {
				count -= 3;
			}
			if(count > 128){
				mark = 1;
			}
			if(count < 4){
				mark = 0;
			}
			
		}
		
	}
	
		
	}

	return 0;
}



