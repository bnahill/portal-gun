#include <pic12f683.h>

#include "portal-gun.h"
#include "button.h"

// GP0
#define BUTTON_PIN_MASK     (0b00000001)
#define BUTTON_IF_REG       INTCON
#define BUTTON_IF_MASK      (0b00000001)
#define BUTTON_IE_REG       INTCON
#define BUTTON_IE_MASK      (0b00001000)

#define DEBOUNCE_THRESH  ((unsigned int) 100)
#define RELEASE_THRESH   ((unsigned int) 40)
#define POWER_THRESH     ((unsigned int) 15000)

// For some reason this can't appear in a header or SDCC dies
extern low_power;

unsigned char pressed = 0;
unsigned char release = 0;
unsigned int debounce = 0;


void prepare_sleep(void);


void button_isr(void) interrupt{
	if(!(GPIO & BUTTON_PIN_MASK))
		// Probably just woke up. Disable that interrupt
		if(low_power){
			BUTTON_IE_REG &= ~BUTTON_IE_MASK;
			wakeup();
		}
	else {
		// Keep interrupt enabled so it can go when the button
		// is pressed again.
		sleep();
	}
}

void button_init(void){
	// Enable weak pull-ups only for the button
	WPU = BUTTON_PIN_MASK;
	// Enable globally
	OPTION_REG &= ~0x80;
	
	// Set as input
	TRISIO |= BUTTON_PIN_MASK;
	
	// Set interrupt-on-change for button
	IOC = BUTTON_PIN_MASK;
	CCPR1L = 0;
}

void button_run(void){
	if(!(GPIO & BUTTON_PIN_MASK)){
		if (debounce > POWER_THRESH){
			// No need for release for power button
			release = 0;
			debounce = 0;
			prepare_sleep();
		}
		debounce++; 
		release = 0;
	} else if (debounce > DEBOUNCE_THRESH){
		if(++release > RELEASE_THRESH){
			release = 0;
			debounce = 0;
			color_toggle();
		}
	} else if (debounce){
		debounce -= 1;
	}
}

void button_sleep(void){
	
}

void prepare_sleep(void){
	// Make it look like it's sleeping
	pwm_sleep();
	// Enable that interrupt
	BUTTON_IF_REG &= ~BUTTON_IF_MASK;
	BUTTON_IE_REG |= BUTTON_IE_MASK;
	// Actually sleep on the IOC interrupt
}

