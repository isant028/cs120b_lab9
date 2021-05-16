/*	Author: Isabella Santiago
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 9  Exercise 4
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
unsigned char button = 0x00;
unsigned char buttonUp = 0x00;
unsigned char buttonDown = 0x00; 

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

enum com_states { com_on, com_off} com_state;
void CombineLEDsSM(){
    switch (com_state)
    {
    case com_on:
        com_state = com_off;  
        break;
    case com_off:
        if(button){ //when button is pushed go to on state, else stay here
            com_state = com_on;
        }
        else{
            com_state = com_off; 
        }
        break;
  
    }
    
    switch (com_state)
    {
    case com_on:
        PORTB = 0x10 |  threeLEDs | blinkingLED; //when button is pushed turn on speaker
        break;
    case com_off:
        PORTB = 0x00 |  threeLEDs | blinkingLED; 
        break;
  
    }

}
unsigned int timer = 1;
enum sound_states { start, wait, up, wait1, down, wait2} sound_state;
void FreqeuncySM(){
    switch (sound_state){
        case start:
            sound_state = wait; 
            break; 
        case wait:
            if(buttonUp){ //when first button is pressed, go to first wait to go up frequency 
                sound_state = wait1;
            }
            else if (buttonDown){
                sound_state = wait2; //when sec button is pressed, go to sec wait to go down frequency 
            }
            else{
                sound_state = wait; //else stay here
            }
            break; 

        case wait1:
            if (buttonUp){ //while button is still being pressed, stay here
                sound_state = wait1;
            }
            else{
                sound_state = up; //once button is released, go to up state
            }
            break; 
       
        case wait2:
            if (buttonDown){ //while button is still being pressed, stay here
                sound_state = wait2;
            }
            else{
                sound_state = down; //once button is released, go to down state
            }
            break; 
        
        case up:
            sound_state = wait; //go back to wait to wait for next input
            break; 

        case down:
            sound_state = wait; //go back to wait state to wait for next input
            break; 
        
    }
     switch (sound_state){
        case start:
            timer = 0; //everytime go back to start state, reset timer. 
            break; 

        case wait:
            break; 
       
       case wait1:
            break;
        
        case wait2:

            break; 
        
        case up: 
            {
                timer++; 
            }
            break; 

        case down:
            if (timer > 1) {//to not go below zero
                timer--; 
            }
            break; 
        
    }

}

unsigned int time3 = 300;
unsigned int time1 = 1000; 
unsigned int sound = 2; 
 

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(1);
	TimerOn();
    led_state = state1;
    blink_state = on; 
    com_state = com_off;  
    sound_state = start; 

	while(1) {
        button = ~PINA & 0x04;
        buttonUp = ~PINA & 0x01;
        buttonDown = ~PINA & 0x02;
        if(time3 >= 300){ //every 300 ms this will tick
            ThreeLEDsSM();
            time3 = 0; 
        }
		if(time1 >= 1000){ //every second this will tick
            BlinkingLEDSM();
            time1 = 0;
        }

        if(sound > timer){ //only tick at max every 1 ms
            CombineLEDsSM();
            sound = 0;
        }
        FreqeuncySM();
        
		while(!TimerFlag);
		TimerFlag = 0;
        time3 += 1;
        time1 += 1; 
        sound += 1; 
	}
    return  1; 
}

