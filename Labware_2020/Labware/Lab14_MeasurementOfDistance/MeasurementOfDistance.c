// MeasurementOfDistance.c
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to periodically initiate a software-
// triggered ADC conversion, convert the sample to a fixed-
// point decimal distance, and store the result in a mailbox.
// The foreground thread takes the result from the mailbox,
// converts the result to a string, and prints it to the
// Nokia5110 LCD.  The display is optional.
// January 15, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// Slide pot pin 3 connected to +3.3V
// Slide pot pin 2 connected to PE2(Ain1) and PD3
// Slide pot pin 1 connected to ground


#include "ADC.h"
#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "TExaS.h"
#include <stdio.h>
#include <string.h>

void EnableInterrupts(void);  // Enable interrupts

unsigned char String[10]; // null-terminated ASCII string
unsigned long Distance;   // units 0.001 cm
unsigned long ADCdata;    // 12-bit 0 to 4095 sample
unsigned long Flag;       // 1 means valid Distance, 0 means Distance is empty

//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).  Calibration
// data is gathered using known distances and reading the
// ADC value measured on PE1.  
// Overflow and dropout should be considered 
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
unsigned long Convert(unsigned long sample){
  return 0.733*sample + 0.669;
}

// Initialize SysTick interrupts to trigger at 40 Hz, 25 ms
void SysTick_Init(unsigned long period){
	
	NVIC_ST_CTRL_R = 0;           // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;  // reload value for 40Hz frequency (assuming 80MHz) //1999999
															  //	If clock = 80M, cycle time = 1/80M = 12.5ns
																//Systick should interrupt every 1/40 = 25 ms ? SysTick interrupts to sample at 40 Hz
																//Systick count * 12.5ns should be = 25 ms
																//Scount = 25ms/12.5ns = 2000000
	
  NVIC_ST_CURRENT_R = 0;        // any write to current clears it
  NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; // priority 0               
  NVIC_ST_CTRL_R = 0x00000007;  // enable with core clock and interrupts
}

// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void){ 
	ADCdata = ADC0_In(); 
	Distance = Convert(ADCdata);	

	Flag=1;
}

//-----------------------UART_ConvertDistance-----------------------
// Converts a 32-bit distance into an ASCII string
// Input: 32-bit number to be converted (resolution 0.001cm)
// Output: store the conversion in global variable String[10]
// Fixed format 1 digit, point, 3 digits, space, units, null termination
// Examples
//    4 to "0.004 cm"  
//   31 to "0.031 cm" 
//  102 to "0.102 cm" 
// 2210 to "2.210 cm"
//10000 to "*.*** cm"  any value larger than 9999 converted to "*.*** cm"
void UART_ConvertDistance(unsigned long n){
// as part of Lab 11 you implemented this function
	if(n > 9999){
		sprintf((char*)String, "*.*** cm");
	}else {
		if((n / 10) == 0){
			sprintf((char*)String, "0.00%d cm", (int)n);
		}else if (((n / 10) > 0) && ((n / 10) < 10)){
			sprintf((char*)String, "0.0%d cm", (int)n);
		}else if (((n / 10) >= 10) && ((n / 10) < 100)){
			sprintf((char*)String, "0.%d cm", (int)n);
		}else if ((n / 10) >= 100){
			if(n%1000 < 10){
				sprintf((char*)String, "%d.00%d cm", (int)n/1000, (int)n%1000);
			}else if ((n%1000 >= 10) && (n%1000 < 100)){
				sprintf((char*)String, "%d.0%d cm", (int)n/1000, (int)n%1000);
			}else if (n%1000 >= 100){
				sprintf((char*)String, "%d.%d cm", (int)n/1000, (int)n%1000);
			}	
		}
	}
}

void UART_OutChar(unsigned char data){
// as part of Lab 11, modify this program to use UART0 instead of UART1
  while((UART0_FR_R&UART_FR_TXFF) != 0);
  UART0_DR_R = data;
}

void UART_OutString(unsigned char buffer[]){
// as part of Lab 11 implement this function
	int i = 0;
	int k = strlen((const char*)buffer);
	for(i = 0; i < k; i++){
		UART_OutChar(buffer[i]);
	}
}

// once the ADC and convert to distance functions are operational,
// you should use this main to build the final solution with interrupts and mailbox
int main(void){ 
  volatile unsigned long delay;
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
	ADC0_Init();					 // initialize ADC0, channel 1, sequencer 3
	Nokia5110_Init();			 // initialize Nokia5110 LCD (optional)
	SysTick_Init(2000000); // initialize SysTick for 40 Hz interrupts
  EnableInterrupts();
	
  while(1){ 
		if(Flag==1)
		{	
			UART_ConvertDistance(Distance);
			
			Nokia5110_SetCursor(0, 0);
			Nokia5110_OutString(String); 
			//UART_OutString(String); 
			//UART_OutChar('\n'); //output to uart
			Flag=0;	
		}
  }
}
