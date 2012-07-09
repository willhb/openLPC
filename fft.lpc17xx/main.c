#include <stdio.h>
#include <lpc17xx/lpc17xx.h>
#include <lpc17xx/openlpc.h>
#include <cr_dsplib.h>
#include <math.h>



/* Enumerate number of FFT points to be tested */
#define NPOINTS_64			64
#define NPOINTS_256			256
#define NPOINTS_1024		1024
#define NPOINTS_4096		4096

/* Define which function to test */
#define NPOINTS				NPOINTS_1024

/* Test functions */
static int iTest_DcInput(void);
static int iTest_UnitImpulse(void);
static int iTest_SinusoidInput(void);
static void vTest_PerformFFT(void);

/* Storage for FFT input and output values. Complex values are used consisting
   of real and imaginary parts, hence the buffers are twice the number of
   points */
//static short *psi_Input  = (short *)0x2007C000; /* AHB SRAM0 */
//static short *psi_Output = (short *)0x20080000; /* AHB SRAM1 */

short buffer_input[2048];
short buffer_output[2048];

static short *psi_Input;
static short *psi_Output;



/*****************************************************************************
** Function name:   main
**
** Descriptions:    Test bench for FFT functions
**
** Parameters:	    None
**
** Returned value:  None
**
******************************************************************************/
int main(void)
{
	
	
	psi_Input  = &buffer_input;
	psi_Output  = &buffer_output;
	
	printf("Test \n\r");
	
	static volatile int i = 0;

	if (iTest_DcInput() == 0)
	{
		
		printf("DC test failed\n\r");
		/* Test failed */
		while(1)
		{
			i++;
		}
	}
	
	printf("\n\r");
	
	if (iTest_UnitImpulse() == 0)
	{
		/* Test failed */
		
		printf("Unit Impulse test failed.\n\r");
		
		while(1)
		{
			i++;
		}
	}
	
	printf("\n\r");
	
	if (iTest_SinusoidInput() == 0)
	{
		printf("Sine test failed\n\r");
		while(1)
		{
			i++;
		}
	}
	
	printf("\n\r");

	printf("ALl tests passed, entering loop.\n\r");
	/* All tests passed */
	while(1)
	{
		i++;
	}
}

/*****************************************************************************
** Function name:   iTest_DcInput
**
** Descriptions:    Test FFT function with a DC input
**
** Parameters:	    None
**
** Returned value:  Zero for failure, otherwise pass
**
******************************************************************************/
static int iTest_DcInput(void)
{
	int j;
	int iMagnitude;

	/* Create DC input data */
	for(j = 0; j < NPOINTS; j++)
	{
		/* Real part - constant (DC) value */
		psi_Input[2*j] = 0x4000;
		/* Immaginary part */
		psi_Input[(2*j)+1] = 0;
	}

	/* Perform defined FFT function */
	vTest_PerformFFT();

	/* DC input should result in a large positive value for the magnitude
	   of output 0 and a value of 0 for all other outputs */
	for(j = 0; j < NPOINTS; j++)
	{
		
		
		iMagnitude = (int)(pow(psi_Output[2 * j], 2) +
						   pow(psi_Output[(2 * j) + 1], 2));

		printf("%d ", iMagnitude);

		if (j == 0)
		{
			if (iMagnitude <= 0)
			{
				/* Test has failed */
				return 0;
			}
		}
		else
		{
			if (iMagnitude > 0)
			{
				/* Test has failed */
				return 0;
			}
		}
	}
	return 1;
}

/*****************************************************************************
** Function name:   iTest_UnitImpulse
**
** Descriptions:    Test FFT function with a unit impulse input
**
** Parameters:	    None
**
** Returned value:  Zero for failure, otherwise pass
**
******************************************************************************/
static int iTest_UnitImpulse(void)
{
	int j;
	int iMagnitude;
	int iMagnitude_0;

	/* Create unit impulse input data */
	psi_Input[0] = 0x4000;	/* Real part of first input value is non zero */

	/* All other real and imaginary input values are zero */
	for(j = 1; j < NPOINTS; j++)
	{
		psi_Input[2 * j] = 0;
		psi_Input[(2 * j) + 1] = 0;
	}

	/* Perform defined FFT function */
	vTest_PerformFFT();

	/* Unit impluse input should result in the magnitude of all ouputs having
	   the same non-zero value */
	iMagnitude_0 = (int)(pow(psi_Output[0], 2) + pow(psi_Output[1], 2));

	for(j = 1; j < NPOINTS; j++)
	{
		iMagnitude = (int)(pow(psi_Output[2 * j], 2) +
						   pow(psi_Output[(2 * j) + 1], 2));

		printf("%d ", iMagnitude);

		if (iMagnitude != iMagnitude_0)
		{
			/* Test has failed */
			return 0;
		}
	}
	return 1;
}

/*****************************************************************************
** Function name:   iTest_SinusoidInput
**
** Descriptions:    Test FFT function with a sinusoidal input
**
** Parameters:	    None
**
** Returned value:  Zero for failure, otherwise pass
**
******************************************************************************/
static int iTest_SinusoidInput(void)
{
	int j;
	int iMagnitude;
	double dTemp;

	/* Create sinusoidal input data */
	double dPI = 4.0f * atan(1.0f);

	/* Frequency of input sinusoid is a fraction of the
       sampling frequency, in this case Fout = Fs * (2/NPOINTS) */
	double dOmega = 2 * dPI * (2.0f / NPOINTS);

	for(j = 0; j < NPOINTS; j++)
	{
		dTemp = pow(2.0f, 14) * cos(dOmega * j);
		/* Real part of value n */
		psi_Input[2 * j] = (short int)dTemp;
		/* Immaginary part of value n */
		psi_Input[(2 * j) + 1] = 0;
	}

	/* Perform defined FFT function */
	vTest_PerformFFT();

	/* Frequency of sinusoid input is Fs*(2/NPOINTS) so magnitude of output
           value corresponding to bin[2] and bin[NPOINTS-2] should be large non
           zero values. */
	for(j = 0; j < NPOINTS; j++)
	{
		iMagnitude = (int)(pow(psi_Output[2 * j], 2) +
						   pow(psi_Output[(2 * j) + 1], 2));

		printf("%d ", iMagnitude);

		if ((j == 2) || (j == (NPOINTS - 2)))
		{
			if (iMagnitude < 255)
			{
				/* Test has failed */
				return 0;
			}
		}
		else
		{
			if (iMagnitude > 255)
			{
				/* Test has failed */
				return 0;
			}
		}
	}
	return 1;
}

/*****************************************************************************
** Function name:   vTest_PerformFFT
**
** Descriptions:    Perform one of the FFT functions
**
** Parameters:	    None
**
** Returned value:  None
**
******************************************************************************/
static void vTest_PerformFFT(void)
{
	#ifdef NPOINTS
	{
		#if (NPOINTS == NPOINTS_64)
		{
			vF_dspl_fftR4b16N64(psi_Output, psi_Input);
		}
		#elif (NPOINTS == NPOINTS_256)
		{
			vF_dspl_fftR4b16N256(psi_Output, psi_Input);
		}
		#elif (NPOINTS == NPOINTS_1024)
		{
			vF_dspl_fftR4b16N1024(psi_Output, psi_Input);
		}
		#elif (NPOINTS == NPOINTS_4096)
		{
			vF_dspl_fftR4b16N4096(psi_Output, psi_Input);
		}
		#else
		{
			#error "NPOINTS Not Valid!"
		}
		#endif
	}
	#else
	{
		#error "NPOINTS Not Defined!"
	}
	#endif
}

/*****************************************************************************
 **                            End Of File
 *****************************************************************************/


