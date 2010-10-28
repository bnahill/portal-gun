#include <pic12f683.h>

#include "portal-gun.h"
#include "rotary.h"

#define ROTARY_PIN_MASK    (0b00010000)
// AN3, in register position
#define ROTARY_CHAN_MASK   (0b11 << 2)
// AN3 only
#define ROTARY_ANSEL_MASK  (0b00001000)


#define ABS(a)	   (((a) < 0) ? -(a) : (a))
// Number of samples required before we start caring
#define WARMUP     250
#define COOLDOWN   100

unsigned char prev, prev2, prev3, prev4, prev5, prev6, prev7;

char is_changing;
// Until average is built
unsigned char warmup;
char cooldown;

void rotary_init(void){
	warmup = WARMUP;
	// Set as input
	TRISIO |= ROTARY_PIN_MASK;

	// Use Fosc / 32
	ANSEL = (0b010 << 4) | ROTARY_ANSEL_MASK;
	
	// Turn that shit on!
	ADCON0 = 0b00000001 | ROTARY_CHAN_MASK;
}

void rotary_sleep(void){
	warmup = WARMUP;
	is_changing = 0;
}

void rotary_read(void){
	register unsigned char value;
	register unsigned char avg1, avg2;
	// Start conversion
	ADCON0 |= 0b00000010;
	// Wait for done
	while(ADCON0 & 0b00000010);
	value = ADRESH;
	// If still warming up, just keep loading the average.
	if(warmup){
		warmup = warmup - 1;
		prev7 = prev6;
		prev6 = prev5;
		prev5 = prev4;
		prev4 = prev3;
		prev3 = prev2;
		prev2 = prev;
		prev = value;
		return;
	}
	prev7 = prev6;
	prev6 = prev5;
	prev5 = prev4;
	prev4 = prev3;
	prev3 = prev2;
	prev2 = prev;
	prev = value;
	avg1 = (value >> 2) + (prev2 >> 2) + (prev3 >> 2) + (prev4 >> 2);
	avg2 = (prev4 >> 2) + (prev5 >> 2) + (prev6 >> 2) + (prev7 >> 2);
	if(ABS((signed long)((unsigned long)avg1 - (unsigned long)avg2)) > 2){
		CCPR1L = value;
		//pwm_set(value);
	}
}
