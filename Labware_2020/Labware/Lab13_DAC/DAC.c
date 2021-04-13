// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC

#include "DAC.h"
#include "..//tm4c123gh6pm.h"

// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void){
		volatile unsigned long delay;
		SYSCTL_RCGC2_R |= 0x02;          // unlock port B clock
    delay = SYSCTL_RCGC2_R;          // allow time for clock to start
    GPIO_PORTB_CR_R |= 0x0F;         // allow change to PB3-PB0
    GPIO_PORTB_AMSEL_R &= ~0x0F;     // disable analog function on PB3-PB0
    GPIO_PORTB_PCTL_R = 0x00;        // clear PCTL register on PB3-PB0
    GPIO_PORTB_DIR_R |= 0x0F;        // PB3-PB0 outputs
    GPIO_PORTB_AFSEL_R &= ~0x0F;     // disable alternate function on PB3-PB0
    GPIO_PORTB_DEN_R |= 0x0F;        // enable digital pins on PB3-PB0
    GPIO_PORTB_DR8R_R |= 0x0F;       // enable 8-mA drive select on PB3-PB
}


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data){
		GPIO_PORTB_DATA_R = data;
}
