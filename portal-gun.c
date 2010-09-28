/*
 * Portal gun light controller for PIC12F683
 * 
 * Wiring:
 *  AN0 (pin 2, RA0) tied to potentiometer
 *  AN0 (pin 7, GP0) is the color button (and serial data)
 *  AN1 (pin 6, GP1) is the power button (and serial clk)
 *  GP5 (pin 2) is the LED select pin (1 drives blue)
 *  CCP1 (pin 5, GP2) is the master LED driver
 *
 */
#include <pic18f2685.h>
#include "button.h"
#include "rotary.h"

#define PERIOD 0x7F


void timer_init(void);


code char at __CONFIG1H CONFIG1H = _OSC_IRCIO67_1H & _FCMEN_OFF_1H & _IESO_OFF_1H;
code char at __CONFIG2L CONFIG2L = _PWRT_OFF_2L & _BOREN_OFF_2L & _BORV_3_2L;
code char at __CONFIG2H CONFIG2H = _WDT_OFF_2H & _WDTPS_32768_2H;
code char at __CONFIG3H CONFIG3H = _PBADEN_OFF_3H & _LPT1OSC_OFF_3H & _MCLRE_ON_3H;
code char at __CONFIG4L CONFIG4L = _STVREN_ON_4L & _LVP_ON_4L & _BBSIZ_1024_4L & _XINST_ON_4L & _DEBUG_ON_4L;
/*
code char at __CONFIG5L CONFIG5L = _CP0_OFF_5L & _CP1_OFF_5L & _CP2_OFF_5L & _CP3_OFF_5L & _CP4_OFF_5L & _CP5_OFF_5L;
code char at __CONFIG5H CONFIG5H = _CPB_OFF_5H & _CPD_OFF_5H;
code char at __CONFIG6L CONFIG6L = _WRT0_OFF_6L & _WRT1_OFF_6L & _WRT2_OFF_6L & _WRT3_OFF_6L & _WRT4_OFF_6L & _WRT5_OFF_6L;
code char at __CONFIG6H CONFIG6H = _WRTC_OFF_6H & _WRTB_OFF_6H & _WRTD_OFF_6H;
code char at __CONFIG7L CONFIG7L = _EBTR0_OFF_7L & _EBTR1_OFF_7L & _EBTR2_OFF_7L & _EBTR3_OFF_7L & _EBTR4_OFF_7L & _EBTR5_OFF_7L;
code char at __CONFIG7H CONFIG7H = _EBTRB_OFF_7H;
*/
char value = 0;

static void isr(void) __interrupt 1{
	//if(INTCON & 0x04){ //Timer interrupt
		LATAbits.LATA6 = 1;
		//Clear IF
		INTCON &= ~0x04;
		PIR1 &= ~0x02;
		LATAbits.LATA6 = 0;
	//}
}
static void isr2(void) __interrupt 2{
	//if(INTCON & 0x04){ //Timer interrupt
	LATAbits.LATA6 = 1;
		//Clear IF
	INTCON &= ~0x04;
	LATAbits.LATA6 = 0;
	//}
}

//Initialize Timer2 for PWM output
void pwm_init() {
	TMR2 = 0;
	//      1:2 postscale      OFF       1:1 prescale
	T2CON = (0b0001 << 3) + (0b0 << 2) + (0b00 << 0);
	// Clear TMR2IF
	PIR1 &= ~0x02;
	// Set comparison period
	PR2 = 0xFF;
	// Set TMR2IE
	PIE1 |= 0x02;
	//           ON
	T2CON |= (0b1 << 2);
}

void main() {
	OSCCON = 0x73;
	OSCTUNE = 0x40;
	//OSCCONbits.IRCF0 = 1; // Don't scale this shit down
	//OSCCONbits.IRCF1 = 1;
	//OSCCONbits.IRCF2 = 1;
	//OSCTUNEbits.PLLEN = 1;
	// Remind it to sleep, rather than just go idle when put to sleep
	//OSCCONbits.IDLEN = 0;
	
	TRISAbits.TRISA6 = 0;
	LATAbits.LATA6 = 0;
	timer_init();
	pwm_init();
	
	// Set GIE
	INTCON |= 0x80;

	//T1CON = 0;
	//TRISIO &= ~0x20;
	//GPIO |= 0x20;
    //pwm_init();
	//adc_init();
	while(1){
		if(INTCON & 0x08){ //Timer interrupt
			LATAbits.LATA6 = 1;
			//Clear IF
			INTCON &= ~0x04;
			_asm
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
				nop
			_endasm;
			LATAbits.LATA6 = 0;
		}
	}
}

/*
 * This should only be called to enter sleep.
 * Return will take ISR
 */
void toggle_power(void){
	// Give other modules an opportunity to clean up
	rotary_sleep();
	_asm
			sleep
	_endasm;
}

void timer_init(void){
	INTCON = 0x00;
	INTCON = 0x60;
	//Configure timer
	T0CON = 0b01001000;
	INTCON = 0x60;
	//Turn timer on
	T0CON = 0b11001000;
	INTCON = 0x60;
}
