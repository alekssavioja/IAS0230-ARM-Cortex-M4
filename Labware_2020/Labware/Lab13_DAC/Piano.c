// Piano.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// There are four keys in the piano
// Daniel Valvano
// December 29, 2014

// Port E bits 3-0 have 4 piano keys

#include "Piano.h"
#include "..//tm4c123gh6pm.h"



// **************Piano_Init*********************
// Initialize piano key inputs
// Input: none
// Output: none
void Piano_Init(void){ 
    volatile unsigned long delay;
		SYSCTL_RCGC2_R |= 0x10;          // unlock port E clock
    delay = SYSCTL_RCGC2_R;          // allow time for clock to start
    GPIO_PORTE_CR_R |= 0x0F;         // allow change to PE3-PE0
    GPIO_PORTE_AMSEL_R &= ~0x0F;     // disable analog function on PE3-PE0
    GPIO_PORTE_PCTL_R = 0x00;        // clear PCTL register on PE3-PE0
    GPIO_PORTE_DIR_R &= ~0x0F;       // PE3-PE0 inputs
    GPIO_PORTE_AFSEL_R &= ~0x0F;     // disable alternate function on PE3-PE0
    GPIO_PORTE_PUR_R &= ~0x0F;       // disable pull-up resistors on PE3-PE0 -- Positive logic
    GPIO_PORTE_DEN_R |= 0x0F;        // enable digital pins on PE3-PE0
}
// **************Piano_In*********************
// Input from piano key inputs
// Input: none 
// Output: 0 to 15 depending on keys
// 0x01 is key 0 pressed, 0x02 is key 1 pressed,
// 0x04 is key 2 pressed, 0x08 is key 3 pressed
unsigned long Piano_In(void){
		unsigned long value;
	
		switch(GPIO_PORTE_DATA_R){
			// Piano key C is pressed
			case 0x01 :
				value = 0;
				break;
			// Piano key D is pressed
			case 0x02 :
				value = 1;
				break;
			// Piano key E is pressed
			case 0x04 :
				value = 2;
				break;
			// Piano key G is pressed
			case 0x08 :
				value = 3;
				break;
			// Nothing is pressed
			default :
				value = 4;
		}
		return value;
}

