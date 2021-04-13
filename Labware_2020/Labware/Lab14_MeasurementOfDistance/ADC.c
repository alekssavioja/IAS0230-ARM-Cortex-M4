// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0 SS3 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result. 
// Daniel Valvano
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

#include "ADC.h"
#include "..//tm4c123gh6pm.h"

// This initialization function sets up the ADC 
// Max sample rate: <=125,000 samples/second
// SS3 triggering event: software trigger
// SS3 1st sample source:  channel 1
// SS3 interrupts: enabled but not promoted to controller
void ADC0_Init(void){
	volatile unsigned long delay;
     
  SYSCTL_RCGC2_R |= 0x00000010; // 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;  
	
  GPIO_PORTE_DIR_R &= ~0x04;    // 2) make PE2 input
  GPIO_PORTE_AFSEL_R |= 0x04;   // 3) enable alternate function on PE2
  GPIO_PORTE_DEN_R &= ~0x04;    // 4) disable digital I/O on PE2
  GPIO_PORTE_AMSEL_R |= 0x04;   // 5) enable analog functionality on PE2
	
	SYSCTL_RCGCADC_R |= 0x0001;		// 6) activate ADC0
	delay = SYSCTL_RCGCADC_R;
	SYSCTL_RCGC0_R |= 0x00010000;  // 7) activate ADC0 
	
	// Configure the ADC0
  ADC0_PC_R &= ~0xF;						// Clearing the register first just in case	
  ADC0_PC_R |= 0x1;             // 7) configure for 125K samples/sec -- 0x7 = 1MHz; 0x5 = 500KHz; 0x3 = 250KHz; 0x1 = 125KHz
  ADC0_SSPRI_R = 0x0123;        // 8) Sequencer 3 is highest priority -- Used in book, most simplest
  ADC0_ACTSS_R &= ~0x0008;      // 9) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;       // 10) seq3 is software trigger -- Pretty much asks when do we want to capture a sample; 0xF means always; 0x0 Means on software demand
																// As i am doing an inverted AND that means that the trigger has a value of 0x0. As sequnecer 3 is the 15th to 12 bits, i must invert those bits
	ADC0_SSMUX3_R &= ~0x000F;
  ADC0_SSMUX3_R += 1; 					// 11) channel Ain1 (PE2)
  ADC0_SSCTL3_R = 0x0006;       // 12) no TS0 D0, yes IE0 END0; END0 = will set a flag on sample completion; IE0 = One sample at a time	
  ADC0_IM_R &= ~0x0008;         // 13) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;       // 14) enable sample sequencer 3
}


//------------ADC0_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
unsigned long ADC0_In(void){  
  unsigned long result;
  ADC0_PSSI_R = 0x0008;            // 1) initiate SS3	-- Seq3 Start sample conversion
  while((ADC0_RIS_R&0x08)==0){};   // 2) wait for conversion to complete -- Setting a flag that shows if the conversion is done
  result = ADC0_SSFIFO3_R&0xFFF;   // 3) read results -- Read data from FIFO buffer, the and in the end is just for caution to get 12bits of data
  ADC0_ISC_R = 0x0008;             // 4) acknowledge completion -- Clear flag
  return result;
}
