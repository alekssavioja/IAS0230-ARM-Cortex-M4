// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13 
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"

/*																																																Needs to be done 16 time so RELOAD/16
Piano key 3: G generates a sinusoidal DACOUT at 783.991 Hz	--	1.275 ms	--	RELOAD = 102000 --  6375
Piano key 2: E generates a sinusoidal DACOUT at 659.255 Hz	--	1.516 ms	--	RELOAD = 121280 --  7580
Piano key 1: D generates a sinusoidal DACOUT at 587.330 Hz  --	1.7 ms		--	RELOAD = 136000 --  8500
Piano key 0: C generates a sinusoidal DACOUT at 523.251 Hz	--  1.911 ms	--	RELOAD = 152880	--	9555
*/
const unsigned long Freq_Value_For_RELOAD[4] = { 9555, 8500, 7580, 6375 };//9550, 8508, 7570, 6366
unsigned long which_button = 0;

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);

int main(void){ // Real Lab13 
	// for the real board grader to work 
	// you must connect PD3 to your DAC output
  TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz
	// PortE used for piano keys, PortB used for DAC        
  Sound_Init(); // initialize SysTick timer and DAC
  Piano_Init();
  EnableInterrupts();  

   while(1){
	        which_button = Piano_In(); // Read input from switches

        if (which_button == 4) {		 // Checks if no button pressed
            Sound_Off(); 						 // Turn off the sound
        }
        else {
            Sound_Tone(Freq_Value_For_RELOAD[which_button]);	// Gives RELOAD value as input depending on which button has been pressed to play certain pitched sounds
        }
	 }        
}

// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec){ 
  unsigned long count;
  while(msec > 0 ) {  // repeat while there are still delay
    count = 16000;    // about 1ms
    while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}


