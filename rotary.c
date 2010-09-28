#include "portal-gun.h"
#include "rotary.h"
#include <pic18f2685.h>

#define ABS(a)	   (((a) < 0) ? -(a) : (a))
// Number of samples required before we start caring
#define WARMUP     25
#define COOLDOWN   25

char average;

char is_changing;
// Until average is built
char warmup;
char cooldown;
RotaryFunc change, save = 0;

void rotary_init(RotaryFunc change_call, RotaryFunc save_call){
	ADCON1 = 0b00001110;//VSS,VDD ref. AN0 analog only
	ADCON0 = 0x00;//clear ADCON0 to select channel 0 (AN0)
	ADCON2 = 0b00001000;//ADCON2 setup: Left justified, Tacq=2Tad, Tad=2*Tosc (or Fosc/2)
	ADCON0bits.ADON = 0x01;//Enable A/D module
	change = change_call;
	save = save_call;
	is_changing = 0;
	warmup = WARMUP;
}

void rotary_sleep(void){
	warmup = WARMUP;
	is_changing = 0;
}

void rotary_read(void){
	ADCON0bits.GO_DONE = 1;
	while(ADCON0bits.GO_DONE != 0);
	// If still warming up, just keep loading the average.
	if(warmup){
		average = (average >> 1) + (ADRESH >> 1);
		warmup -= 1;
		return;
	}
	if(ABS(ADRESH - average) > 4){
		is_changing = 1;
		cooldown = COOLDOWN;
		change(ADRESH);
	}
	else if(is_changing){
		if(cooldown-- == 0){
			// Done changing
			is_changing = 0;
			// Save the value to EEPROM
			save(ADRESH);
		}
	}
	average = (average >> 1) + (ADRESH >> 1);
}
