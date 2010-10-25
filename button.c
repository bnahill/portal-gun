#include <pic12f683.h>

#include "portal-gun.h"
#include "button.h"

// GP0
#define BUTTON_PIN_MASK     (0b00000001)
#define BUTTON_IF_REG       INTCON
#define BUTTON_IF_MASK      (0b00000001)
#define BUTTON_IE_REG       INTCON
#define BUTTON_IE_MASK      (0b00001000)

#define DEBOUNCE_THRESH  100
#define RELEASE_THRESH   40
#define POWER_THRESH     5000

// For some reason this can't appear in a header or SDCC dies
extern low_power;

unsigned char pressed = 0;
unsigned char release = 0;
unsigned int debounce = 0;


void button_isr(void) interrupt{
	// Probably just woke up. Disable that interrupt
	BUTTON_IE_REG &= ~BUTTON_IE_MASK;
	if(low_power) wakeup();
}

void button_init(void){
	// Set as input
	TRISIO |= BUTTON_PIN_MASK;
	//GPIO |= BUTTON_PIN_MASK;
	// Enable weak pull-ups only for the button
	WPU = BUTTON_PIN_MASK;
	// Enable globally
	OPTION_REG &= ~0x80;
	
	// Enable global (and peripheral) interrupts
	INTCON |= 0x80;
	
	// Set interrupt-on-change for button
	IOC = BUTTON_PIN_MASK;
}

void button_run(void){
	if(GPIO & BUTTON_PIN_MASK){
		debounce++; 
		release = 0;
	} else if (debounce > POWER_THRESH){
		// No need for release for power button
		release = 0;
		debounce = 0;
		sleep();
	} else if (debounce > DEBOUNCE_THRESH){
		if(++release > RELEASE_THRESH){
			release = 0;
			debounce = 0;
			color_toggle();
		}
	} else if (debounce){
		debounce -= 2;
	}
}

void button_sleep(void){
	debounce = 0;
	// Enable that interrupt
	BUTTON_IF_REG &= ~BUTTON_IF_MASK;
	BUTTON_IE_REG |= BUTTON_IE_MASK;
}

