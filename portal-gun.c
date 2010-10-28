/*
 * Portal gun light controller for PIC12F683
 * 
 * Since the external interrupt connection is taken for 
 * the PWM output, low-power mode is just where the timer
 * is disabled and output driven low.
 *
 * Wiring:
 *  AN3 (pin 3, GP4) tied to potentiometer
 *  AN0 (pin 7, GP0) is the button (and serial data)
 *  GP1 (pin 6) is the blue LED pin
 *  GP5 (pin 2) is the orange LED pin
 *  CCP1 (pin 5, GP2) is the master LED driver
 *
 */
#include <pic12f683.h>

#include "portal-gun.h"
#include "button.h"
#include "rotary.h"

#define PERIOD 0x7F

#define BLUE_PIN_MASK   (0b00000010)
#define ORANGE_PIN_MASK (0b00100000)

#define PWM_PIN_MASK    (0b00000100)
#define TMR2_IF_REG     PIR1
#define TMR2_IF_MASK    (0b00000010)
#define TMR2_IE_REG     PIE1
#define TMR2_IE_MASK    (0b00000010)

char low_power = 0;

void lights_init(void);


typedef unsigned int word;
word at 0x2007 CONFIG = _INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_OFF & _MCLRE_ON & _CP_OFF & _CPD_OFF & _BOD_ON & _IESO_ON & _FCMEN_ON;


enum {BLUE = 0x00, ORANGE = 0x01} color = BLUE;
char values[2] = {190, 255};

//Initialize Timer2 for PWM output
void pwm_init() {
	// Disable output
	TRISIO |= PWM_PIN_MASK;
	// Set timer period
	PR2 = 0xFF;
	// Configure for PWM mode with 0b11 in LSB of value
	CCP1CON = 0b00111100;
	// Provide initial value
	CCPR1L = 0x7F >> 1;
	// Clear TMR2 IF
	TMR2_IF_REG &= ~TMR2_IF_MASK;
	// Enable T2 with 1:4 prescaler
	T2CON = 0b00000101;
	
	// Wait for a new period
	while(!(TMR2_IF_REG & TMR2_IF_MASK));
	// Enable output
	TRISIO &= ~PWM_PIN_MASK;
}

void color_toggle(void){
	color ^= 0x01;
	if(color == BLUE){
		GPIO = BLUE_PIN_MASK;
		CCPR1L = values[BLUE];
	} else {
		GPIO = ORANGE_PIN_MASK;
		CCPR1L = values[ORANGE];
	}
}

void pwm_sleep(void){
	// Disable T2
	T2CON &= ~0x04;
	CCP1CON &= ~0x0F;
	// Drive output to ground
	GPIO &= ~PWM_PIN_MASK;
}

void pwm_wakeup(void){
	// Start T2 again
	T2CON |= 0x04;
	CCP1CON |= 0x0C;
}

void main() {
	int i;
	// Disable comparator everywhere
	CMCON0 |= 0b00000111;
	// Disable all analog
	ANSEL = 0x00;
	// Enable global (and peripheral) interrupts
	INTCON |= 0x80;
	
	TRISIO &= ~(ORANGE_PIN_MASK | BLUE_PIN_MASK);
	
	pwm_init();
	//rotary_init();
	button_init();
	while(1){
		for(i = 0; i < 5; i++);
		//rotary_read();
		button_run();
	}
}

/*
 * This should only be called to enter sleep.
 * Return will take ISR
 */
void sleep(void){
	low_power = 1;
	GPIO &= ~(ORANGE_PIN_MASK | BLUE_PIN_MASK);
	// Set to 32kHz internal oscillator
	OSCCON &= ~0b111;
	_asm
		sleep;
	_endasm;
}


void wakeup(void){
	// Set to 4MHz, assuming coming from 32k
	OSCCON |= 0b110;
	low_power = 0;
	if(color == BLUE){
		GPIO = BLUE_PIN_MASK;
		CCPR1L = values[BLUE];
	} else {
		GPIO = ORANGE_PIN_MASK;
		CCPR1L = values[ORANGE];
	}
	pwm_wakeup();
}

void lights_init(void){
	TRISIO &= ~(BLUE_PIN_MASK | ORANGE_PIN_MASK);
}