/*	Author: Isabella Santiago
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 9  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 * DEMO LINK: 
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;


unsigned long _avr_timer_M = 1; 
unsigned long _avr_timer_cntcurr = 0; 

void TimerOn() {
  	TCCR1B = 0x0B;
  	OCR1A = 125;
  	TIMSK1 = 0x02;
  	TCNT1 = 0;
  	_avr_timer_cntcurr = _avr_timer_M;
  	SREG |= 0x80;
}

void TimerOff() {
  	TCCR1B = 0x00;
}

void TimerISR() {
  	TimerFlag = 1;
}


ISR(TIMER1_COMPA_vect) {

	_avr_timer_cntcurr--; 
	if (_avr_timer_cntcurr == 0) { 
		TimerISR(); 
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

unsigned char threeLEDs = 0x00; 

enum led_states {state1, state2, state3} led_state;
void ThreeLEDsSM() {
	switch (led_state){
        case state1:
            led_state = state2;
            break; 
        
        case state2:
            led_state = state3;
            break;

        case state3:
            led_state = state1;
            break;

        default:
            led_state = state1;
            break; 
    }

    switch (led_state){
        case state1:
            threeLEDs = 0x01;
            break; 
        
        case state2:
            threeLEDs = 0x02;
            break;

        case state3:
            threeLEDs = 0x04; 
            break;

        default:
            break; 
    }
}

unsigned char blinkingLED = 0x00; 
enum blink_states {on, off} blink_state;
void BlinkingLEDSM(){
    switch (blink_state){
        case on:
            blink_state = off;
            break;
        
        case off:
            blink_state = on;
            break;

        default:
            blink_state = on; 
            break; 
    }

    switch (blink_state){
        case on:
            blinkingLED = 0x08;
            break;
        
        case off:
            blinkingLED = 0x00;
            break;

        default:
            blinkingLED = 0x00;
            break; 
    }

}

enum com_states {comb} com_state;
void CombineLEDsSM(){
    switch (com_state)
    {
    case comb:
        com_state = comb;  
        break;
    
    default:
        com_state = comb;  
        break;
    }
    
    switch (com_state)
    {
    case comb:
        PORTB = threeLEDs | blinkingLED; 
        break;
    
    default:
        break;
    }

}

int main(void) {
	//DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(1000);
	TimerOn();
    led_state = state1;
    blink_state = on; 
    com_state = comb;  

	while(1) {
		ThreeLEDsSM();
        BlinkingLEDSM();
        CombineLEDsSM();
		while(!TimerFlag);
		TimerFlag = 0;
	}
    return  1; 
}

