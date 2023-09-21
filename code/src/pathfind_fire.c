//Standard includes
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "global.h"

//Variables
int wait = 0;
int fire_flag = 0;
uint16_t adcRead(uint8_t adcChannel);
uint16_t adcResult[6];
uint32_t run_pt_counter = 1;

void pathfind_fire_start(void)
{
	//Setting up pins for phototransistors
	TRIG_DDR |= (1 << TRIG1_pin | 1 << TRIG2_pin | 1 << TRIG3_pin);
	DDRB |= (1<<Out1 | 1<<Out45);
	DDRD |= (1<<Out23);
	//Setting up pin for the pump
	DDRD |= ( 1<<PUMP);
	//turn pump off
	PORTD &= ~(1<<PUMP);
	//Select Vref = AVcc
	ADMUX = (1<<REFS0);
	//Set prescaler to 128 and turn on the ADC module
	ADCSRA |= (1<<ADPS2 | 1<<ADPS1|1<<ADPS0 | 1<<ADEN);
	//set pin for the 1st Out on
	PORTB |= (1<<Out1);
}

void pathfind_fire(void){
	//add 1 to the cycle counter
	run_path_fire_counter++;
	//start of the ir detetion
	//every 15000 cycles of code it jumps from 1 pair of IR sensors to the next one(the 1st sensor it is by its own)
	//1st IR sensor
	//jump into this if, if the run_path_fire_counter has done 15000 cycles and run_pt_counter is equal to 1 do the following:
	if (run_path_fire_counter%15000 == 0 && run_pt_counter == 1){
		//1st ADC result equals the read from ADC in port in124
		adcResult[1] = adcRead(in124);
		//if the value is higher or lower than the threshold set current_state to state_extinguish
		if (adcResult[1] > FIRE_U || adcResult[1] < FIRE_D){
			current_state = state_extinguish;
		}
		//turn off pin for this phototransistor
		PORTB &= ~(1<<Out1);
		//set pins for the next phototransistors
		PORTD |= (1<<Out23);
		run_pt_counter ++;
		run_path_fire_counter++; //so it doesn't get into the next one
	}
	else if (run_path_fire_counter%15000 == 0 && run_pt_counter == 2){
		//2nd ADC result equals the read from ADC in port in124
		adcResult[2] = adcRead(in124);
		//3rd ADC result equals the read from ADC in port in35
		adcResult[3] = adcRead(in35);
		//if either 2 values are higher or lower than the threshold set current_state to state_extinguish
		if (adcResult[2] > FIRE_U || adcResult[3] > FIRE_U || adcResult[2] < FIRE_D || adcResult[3] < FIRE_D){
			current_state = state_extinguish;
		}
		//turn off pin for this phototransistor
		PORTD &= ~(1<<Out23);
		//set pins for the next phototransistors
		PORTB |= (1<<Out45);
		run_pt_counter ++;
		run_path_fire_counter++; //So it doesn't get into the next one
	}
	else if (run_path_fire_counter%15000 == 0 && run_pt_counter == 3){
		//4th ADC result equals the read from ADC in port in124
		adcResult[4] = adcRead(in124);
		//5th ADC result equals the read from ADC in port in35
		adcResult[5] = adcRead(in35);
		//if either 2 values are higher or lower than the threshold set current_state to state_extinguish
		if (adcResult[4] > FIRE_U || adcResult[5] > FIRE_U || adcResult[4] < FIRE_D || adcResult[5] < FIRE_D){
			current_state = state_extinguish;
		}
		//turn off pin for this phototransistor
		PORTD &= ~(1<<Out45);
		//set pins for the 1st phototransistor
		PORTB |= (1<<Out1);
		//restet counter
		run_pt_counter = 1;
		}
	else{
		//if code has not completed 15000 cycles set current_state to state_idle
		current_state = state_idle;
	}
}

void extinguish(){
	//turn on pump
	PORTD |= (1<<PUMP);
	//100ms delay
	_delay_ms(100);
	//turn pump off
	PORTD &= ~(1<<PUMP);
	//100ms delay
	_delay_ms(100);
	//set current_state to state_evacuate
	current_state = state_evacuate;
}

//Evacuate function that gets called when curent_state = state_evacuate
void evacuate(void){ 
	//Set motors to oposite direction
	PORTB |= (1<<DIRECTION_1 | 1<<DIRECTION_2);
	//Set both motors to fast speed
	OCR2A = FAST;
	OCR2B = FAST;
	//Do a delay so the motors go on reverse for 3s
	_delay_ms(3000);
	//Stop the motors
	OCR2A = SLOW;
	OCR2B = SLOW;
	//Set next state to state_shutdown
	current_state = state_shutdown;
}

//From Alin's presentation
uint16_t adcRead(uint8_t adcChannel){
	//Clear any previously used channel, but keep internal reference
	ADMUX &= 0xf0; 
	//Set the desired channel
	ADMUX |= adcChannel; 
	//Start a conversion
	ADCSRA |= (1<<ADSC);
	//Now wait for the conversion to complete
	while ( (ADCSRA & (1<<ADSC)) );
	//Now we have the result, so we return it to the calling function as a 16 bit unsigned int
	return ADC;
}