// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Port_Init(void);					// Port initialization
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}

// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}

// 800000*12.5ns equals 10ms
void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}

struct State {
  unsigned long Light; 
	unsigned long Walk;
	char name[10];
  unsigned long Time;
  unsigned long Next[8];
};

typedef const struct State State_t;
#define west_green 0
#define west_red 1
#define south_green 2
#define south_red 3
#define walk_green 4
#define blink_walk 5
#define blink_walk_2 6
#define walk_red 7


State_t Fsm[10] = {
// OUT	 Walk											000					   001				 	 010				  011				  	 100					101						110						111
  {0x0C, 0x02, "WG-SR", 50,	  {west_green, 		west_green,	  west_red, 	  west_red, 		west_red, 		west_red,	 	 	west_red, 	  west_red} }, // S0) 

  {0x14, 0x02, "WY-SR", 50,	  {walk_red, 			west_green,   south_green,  south_green, 	walk_green, 	walk_green, 	south_green,  south_green} }, // S1) 
	
  {0x21, 0x02, "SG-WR", 50,	  {south_green, 	south_red, 		south_green,  south_red, 		south_red, 		south_red,		south_red, 	  south_red} }, // S2) 

  {0x22, 0x02, "SY-WR", 50, 	{south_red, 		west_green, 	south_green, 	west_green, 	walk_green, 	west_green, 	walk_green,	 	walk_green} }, // S3) 

  {0x24, 0x08, "WalkG", 50, 	{walk_green, 		blink_walk, 	blink_walk,  	blink_walk,  	walk_green, 	blink_walk, 	blink_walk,  	blink_walk} }, // S4) 

  {0x24, 0x02,"BlinkWalk", 50,{blink_walk_2,	blink_walk_2,	blink_walk_2, blink_walk_2,	blink_walk_2,	blink_walk_2, blink_walk_2, blink_walk_2} }, // S5)
	
	{0x24, 0x00,"BlinkWalk2",50,{blink_walk,		walk_red, 		walk_red, 	 	walk_red,	  	walk_red, 		walk_red, 		walk_red,		 	walk_red} },  // S6)

  {0x24, 0x02, "WalkR", 50, 	{walk_red, 			west_green, 	south_green, 	west_green,  	walk_green, 	west_green, 	south_green, 	west_green} }, // S7) 


};



// ***** 3. Subroutines Section *****
unsigned long cState;
unsigned long input;

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
	Port_Init();
  SysTick_Init();
  EnableInterrupts();
	cState = 0; 
  while(1){
     // output based on current state
    GPIO_PORTB_DATA_R = Fsm[cState].Light;
		GPIO_PORTF_DATA_R = Fsm[cState].Walk;
    // wait for time according to state
    SysTick_Wait10ms(Fsm[cState].Time);
    // get input    
    input = GPIO_PORTE_DATA_R&0x07; // Input 0,1,2
    // change the state based on input and current state
    cState = Fsm[cState].Next[input];
  }
}

void Port_Init(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000032;      // 1) F & E & B clock
  delay = SYSCTL_RCGC2_R;            // delay to allow clock to stabilize    
	
	// Pedastrian Output Pins (2Leds) -- Port F
  GPIO_PORTF_AMSEL_R &= 0x00;        // 2) disable analog function
  GPIO_PORTF_PCTL_R &= 0x00;  			 // 3) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R |= 0x0A;          // 4) Pedastrian Walk PF3,1 output -- PF1=Dont_walk -- PF3=Walk 
  GPIO_PORTF_AFSEL_R = 0x00;         // 5) no alternate function
  //GPIO_PORTF_PUR_R = 0x00;           // 6) Deactivate an internal pullup resistor for input	-- Positive logic activated
	GPIO_PORTF_DEN_R = 0x0A;           // 7) enable digital pins
	
	// Vehicle Output Pins (6Leds)-- PORT_B
	GPIO_PORTB_AMSEL_R = 0x00;        // 2) disable analog function
  GPIO_PORTB_PCTL_R = 0x00;   			// 3) GPIO clear bit PCTL
  GPIO_PORTB_DIR_R |= 0x3F;         // 4) PB5-0 output	--	PB5=WestR, PB4=WestY, PB3=WestG -- PB2=SouthR, PB1=SouthY, PB0=SouthG
  GPIO_PORTB_AFSEL_R = 0x00;        // 5) no asssslternate function
  //GPIO_PORTB_PUR_R &= ~0x01;      // 6) Deactivate an internal pullup resistor for input	-- Positive logic activated
	GPIO_PORTB_DEN_R = 0x3F;          // 7) enable digital pins
	
	// Swtiches/Button Input Pis (3Sw) -- PORT_E
	GPIO_PORTE_AMSEL_R = 0x00;        // 2) disable analog function
  GPIO_PORTE_PCTL_R = 0x00;   			// 3) GPIO clear bit PCTL
  GPIO_PORTE_DIR_R &= ~0x07;        // 4) PE2-0 Input	--	PE2=WalkSensor, PE1=SouthSensor, PE0=WestSensor
  GPIO_PORTE_AFSEL_R = 0x00;        // 5) no alternate function
  GPIO_PORTE_PUR_R &= ~0x01;        // 6) Deactivate an internal pullup resistor for input	-- Positive logic activated
	GPIO_PORTE_DEN_R = 0x07;          // 7) enable digital pins PE2-PE0
}

// 001 -- West
// 010 -- South
// 100 -- Walk
