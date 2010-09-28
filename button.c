#include "button.h"
#include "portal-gun.h"
#include <pic18f2685.h>

#define ANALOG_VDD AN3
#define THRESH	5
#define DEBOUNCE_THRESH 15
#define WARMUP_PERIOD 64

typedef struct {
    const char mask;
	ButtonFunc call;
    long avg, low, high, avgcount;
    int debounce;
    char pressed;
} Button;

static Button buttons[2] = {
	//{ 0x01, toggle_mode },
    { 0x02, toggle_power },
};

void button_init(ButtonFunc event_call){
	
}

