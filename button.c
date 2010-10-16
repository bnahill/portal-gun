#include "button.h"
#include "portal-gun.h"
#include <pic18f2685.h>

#define ANALOG_VDD AN3
#define THRESH	5
#define DEBOUNCE_THRESH 15
#define WARMUP_PERIOD 64

typedef struct {
    const char mask;
    int debounce;
    char pressed;
} Button;

void button_isr(void){
	
}

void button_init(void){
	// Enable weak pull-ups
	INTCON2bits.RBPU = 0;
}

