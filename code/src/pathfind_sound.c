/*
THIS FILE IS NOT USED, WAS ONLY TESTED BUT NEVER IMPLEMENTED TO THE LAST CODE
*/

// Standard Includes 
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>
#include <util/delay.h>
#include "global.h"

//define voids used 
void pathfind_sound_start(void);
void pathfind_sound(void);
void mic_ON(void);
void mic_0(void);
void mic_1(void);
void mic_2(void);
void mic_3(void);

//define arrays and variables
int voltage[4];
int alarm_flag;
//function prototypes
uint16_t adc_read(uint8_t adc_channel);
uint16_t adc_result;

void pathfind_sound_start(void){
	DDRD = 0xF0;
	//channels 0-4 are connected to PC0..PC3
	DDRC = 0xF0;
	PORTC = 0x00; //do not use internall pull ups resistors
	// Select Vref= AVcc
	ADMUX = (1<<REFS0);
	//set prescaler to 128 and turn on the ADC module
	ADCSRA = (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN);
}

void pathfind_sound(void){
	//read voltage from the 4 michropones, one at a time
	for(int channel = 4; channel < 8; channel++){
		adc_result = adc_read(channel);
		//store voltage value from reading in voltage array in the chosen order
		voltage[channel] = adc_result*(V_REF/1024.0);
		//check if voltage is higher than threshold
		if (voltage[channel] > VOLTAGE){
			//call mic_ON void
			mic_ON();
		}
	}
	//set current_state to state_pathfind_fire
	current_state = state_pathfind_fire;
}

void mic_ON(void){
	//check if voltage on microphone 0 is the highest
	if ((voltage[0] > voltage[1]) && (voltage[0] > voltage[2]) && (voltage[0] > voltage[3])){
		//call mic_0 void
		mic_0();
	}
	//check if voltage on microphone 1 is the highest
	if ((voltage[1] > voltage[0]) && (voltage[1] > voltage[2]) && (voltage[1] > voltage[3])){
		//call mic_1 void
		mic_1();
	}
	//check if voltage on microphone 2 is the highest
	if ((voltage[2] > voltage[0]) && (voltage[2] > voltage[1]) && (voltage[2] > voltage[3])){
		//call mic_2 void
		mic_2();
	}
	//check if voltage on microphone 3 is the highest
	if ((voltage[3] > voltage[0]) && (voltage[3] > voltage[1]) && (voltage[3] > voltage[2])){
		//call mic_3 void
		mic_3();
	}
	//if 2 microphoes have the same value could break so jump into finding fire
	//some more advanced logic could have been implemented to solve this problem
	else{
		//set current_state to state_pathfind_fire
		current_state = state_pathfind_fire;
	}
}

void mic_0 (void){ 
	//set motors to turn to the right 
	OCR2A = FAST; 
	OCR2B = SLOW; 
	//set current_state to state_idle
	current_state = state_idle;
}

void mic_1 (void){ 
	//set motors to turn to the left 
	OCR2A = FAST; 
	OCR2B = SLOW; 
	//set current_state to state_idle
	current_state = state_idle;
}
void mic_2 (void){ 
	//set motors to turn to the right 
	OCR2A = FAST; 
	OCR2B = SLOW; 
	//set current_state to state_idle
	current_state = state_idle;
}
void mic_3 (void){ 
	//set motors to turn to the left 
	OCR2A = FAST; 
	OCR2B = SLOW; 
	//set current_state to state_idle
	current_state = state_idle;
}

uint16_t adc_read(uint8_t adc_channel){
	ADMUX &= 0xf0; // clear any previously used channel, but keep internal reference
	ADMUX |= adc_channel; // set the desired channel 
	//start a conversion
	ADCSRA |= (1<<ADSC);
	// now wait for the conversion to complete
	while ( (ADCSRA & (1<<ADSC)) );
	// now we have the result, so we return it to the calling function as a 16 bit unsigned int
	return ADC;
}




