// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"
#include "DAC.h"
#include "Piano.h"
#include "..//tm4c123gh6pm.h"

unsigned char cycle = 0;
const unsigned char SIN_Wave[16] = { 0, 1, 2, 5, 8, 10, 13, 14, 15, 14, 13, 10, 8, 5, 2, 1 };

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Sound_Init(void){
		DAC_Init();
	
		NVIC_ST_CTRL_R = 0;           		// disable SysTick during setup
		NVIC_ST_RELOAD_R = 0xFFFFFF;     		// reload value = Maximum size as it will be changed according to key pressed
		NVIC_ST_CURRENT_R = 0;        		// any write to current clears it
		NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; // priority 0                
		NVIC_ST_CTRL_R = 0x00000007;  		// enable with core clock and interrupts
	
		cycle = 0;
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1	= 0xFF FFFF
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period){
		NVIC_ST_RELOAD_R = (period-1) & 0x00FFFFFF;
}


// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
 // this routine stops the sound output
		GPIO_PORTB_DATA_R &= ~0x0F; // clear PB3-PB0
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
		cycle = (cycle + 1) & 0x0F;  // cycles from 0-15
		DAC_Out(SIN_Wave[cycle]);   // output 1 data each interrupt
}
