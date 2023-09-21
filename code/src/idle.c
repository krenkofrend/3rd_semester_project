//Standard includes
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>
#include <util/delay.h>
#include "global.h"

//Define functions 
void startUltrasonic(void);
void idle_start(void);
void idle(void);
void stop(void);
void reverse(void);
void right_turn(void);
void left_turn(void);
void stop(void);
void _180_turn(void);

//Variables to enter distance functions
int dist_0 = 0;
int dist_0_state = 1;
int dist_1 = 0;
int dist_1_state = 1;
int dist_2 = 0;
int dist_2_state = 1;

//Array of the three masks used to handle the ultrasonic sensors trigger pins
const int8_t trigPins[3] = {TRIG1_pin, TRIG2_pin, TRIG3_pin};

//Global variable containing the last signal from an ultrasonic sensor
uint16_t risingICR;

//For storing the distance measured from each ultrasonic sensor
uint16_t dist[3] = {1,0,0};

//Variable containing the index of the current ultrasonic sensor
uint8_t ultrasonicIndex = 0;

//For checking if a measurement has been made in current cycle
uint8_t	pulseIndex;

//initialization of all the nedded ports and timers for idle
void idle_start(void){
	//Setting up pins for the motors
	DDRB |= (1 << DIRECTION_1 | 1 << DIRECTION_2 | 1 << PWM_2);
	DDRD |= (1 << PWM_1);

	//Setting up pins for ultrasonic sensors
	TRIG_DDR |= (1 << TRIG1_pin | 1 << TRIG2_pin | 1 << TRIG3_pin);
	
	//Initial motor directions
	PORTB &= ~(1<<DIRECTION_1 | 1<<DIRECTION_2);

	//Motor timer
	TCCR2A = 0xA3; //0b10100011  
	TCCR2B = 0x05; //0b00000101

	//Call the ultrasonic funtion
	startUltrasonic();
}

//Idle state function
void idle(void){
	//Set the next state
	current_state = state_pathfind_fire;
	//Add 1 to the cycle counter
	run_idle_counter++;
	//Start of the driving algorithm
	//Check if distance is too close to an object, if it is set correponding ultrasonic varibale, "flag", to 1
	//Variable for right ultrasonic sensor (number 2).
	if (dist[2] < TOO_CLOSE_RANGE){
		dist_2 = 1;
	}
	//Variable for left ultrasonic sensor (number 0).
	else if (dist[0] < TOO_CLOSE_RANGE ){
		dist_0 = 1;
	}
	//Variable for center ultrasonic sensor (number 1).
	else if (dist[1] < TOO_CLOSE_RANGE){
		dist_1 = 1;
	}
	//If the variable, "flag", is ON (set to 1), do the following sequence
	//The 3 sequences are almost the same, the only thing that changes is the turning distance and turning direction
	//Sequence for right ultrasonic sensor 
	else if (dist_2 == 1){
		//If state is 1 do the following:
		if (dist_2_state == 1){
			//Call stop function
			stop();
			//Add 1 to the counter so code can get into next state
			dist_2_state ++;
		}
		//if the run_idle_counter has done a cycle equal to STOP and the dist_2_state is equal to 2 do the following:
		//waiting for a cycle STOP in this case to be done is to wait to change the state of the motors
		if (run_idle_counter%STOP == 0 && dist_2_state == 2){
			reverse();
			//Add 1 to run_idle_counter so code does not jump to next state right after leaving this if function
			run_idle_counter++;
			dist_2_state ++;
		}
		if (run_idle_counter%REVERSE == 0 && dist_2_state == 3){
			stop();
			run_idle_counter++;
			dist_2_state ++;
		}
		if (run_idle_counter%STOP == 0 && dist_2_state == 4){
			right_turn();
			run_idle_counter++;
			dist_2_state ++;
		}
		if (run_idle_counter%TURN == 0 && dist_2_state == 5){
			PORTB &= ~(1<<DIRECTION_2);
			stop();
			run_idle_counter++;
			dist_2_state ++;
		}
		if (run_idle_counter%STOP == 0 && dist_2_state == 6){
			run_idle_counter++;
			//Reset counter values
			dist_2_state = 1;
			dist_2 = 0;
		}
	}
	//Sequence for left ultrasonic sensor 
	else if (dist_0 == 1){
		if (dist_0_state == 1){
			stop();
			dist_0_state ++;
		}
		if (run_idle_counter%STOP == 0 && dist_0_state == 2){
			reverse();
			run_idle_counter++;
			dist_0_state ++;
		}
		if (run_idle_counter%REVERSE == 0 && dist_0_state == 3){
			stop();
			run_idle_counter++;
			dist_0_state ++;
		}
		if (run_idle_counter%STOP == 0 && dist_0_state == 4){
			left_turn();
			run_idle_counter++;
			dist_0_state ++;
		}
		if (run_idle_counter%TURN == 0 && dist_0_state == 5){
			PORTB &= ~(1<<DIRECTION_1);
			stop();
			run_idle_counter++;
			dist_0_state ++;
		}
		if (run_idle_counter%STOP == 0 && dist_0_state == 6){
			run_idle_counter++;
			dist_0_state = 1;
			dist_0 = 0;
		}
	}
	//Sequence for center ultrasonic sensor 
	else if (dist_1 == 1){
		if (dist_1_state == 1){
			stop();
			dist_1_state ++;
		}
		if (run_idle_counter%STOP == 0 && dist_1_state == 2){
			reverse();
			run_idle_counter++;
			dist_1_state ++;
		}
		if (run_idle_counter%REVERSE == 0 && dist_1_state == 3){
			stop();
			run_idle_counter++;
			dist_1_state ++;
		}
		if (run_idle_counter%STOP == 0 && dist_1_state == 4){
			_180_turn();
			run_idle_counter++;
			dist_1_state ++;
		}
		if (run_idle_counter%_180_TURN == 0 && dist_1_state == 5){
			PORTB &= ~(1<<DIRECTION_2);
			stop();
			run_idle_counter++;
			dist_1_state ++;
		}
		if (run_idle_counter%STOP == 0 && dist_1_state == 6){
			run_idle_counter++;
			dist_1_state = 1;
			dist_1 = 0;
		}
	}
	//Check if distance from center sensor is closer than F_RANGE to an object, if it is do the following:
	else if (dist[1] < F_RANGE){
		//If the right ultrasonic sensor is closer or equally far away from a wall than the left one do a left turn
		if (dist[0] >= dist[2]){
			OCR2A = SLOW;
			OCR2B = FAST;
		} 
		//Else do a left turn because the right ultrasonic is closer to the wall than the left one
		else {
			OCR2A = FAST;
			OCR2B = SLOW;
		} 
	}
	//Check if distance from right sensor is closer than S_RANGE to an object, if it is do a left turn
	else if (S_RANGE > dist[2]){
		OCR2A = SLOW;
		OCR2B = FAST;
	} 
	//Check if distance from left sensor is closer than S_RANGE to an object, if it is do a right turn
	else if (S_RANGE > dist[0]){
		OCR2A = FAST;
		OCR2B = SLOW;
	}
	//Else means no sensors detect any objects closer than the ranges compared in the above code so keep going straight
	else {
		OCR2A = FAST;
		OCR2B = FAST;
	}
}	


//Functions to stop, reverse, turn right or left, or do a 180 degree turn
void stop(void){ 
	OCR2A = SLOW;
	OCR2B = SLOW;
}
void reverse(void){ 
	//set motors directions to backwards
	PORTB |= (1<<DIRECTION_1 | 1<<DIRECTION_2);
	OCR2A = FAST;
	OCR2B = FAST;
}
void left_turn(void){ 
	//set left motor direction to backwards
	PORTB |= (1<<DIRECTION_1);
	//set right motor direction to forwards
	PORTB &= ~(1<<DIRECTION_2);
	OCR2A = TURN_SPEED;
	OCR2B = TURN_SPEED;
}
void right_turn(void){ 
	//set left motor direction to forwards
	PORTB &= ~(1<<DIRECTION_1);
	//set right motor direction to backwards
	PORTB |= (1<<DIRECTION_2);
	OCR2A = TURN_SPEED;
	OCR2B = TURN_SPEED;
}
void _180_turn(void){ 
	//set left motor direction to forwards
	PORTB &= ~(1<<DIRECTION_1);
	//set right motor direction to forwards
	PORTB |= (1<<DIRECTION_2);
	OCR2A = TURN_SPEED;
	OCR2B = TURN_SPEED;
}

//Function for setting up the timer for distance measurement operation
void startUltrasonic(void){
	//Setting up waveform generation mode to CTC mode, with OCR1A as top
	//Enabling the ICNC1, input capture noise canceler
	//Setting a clk/64 prescaler, for a span of 262 ms and a resolution of 4 us
	//Setting up so the input capture interrupt is called on a rising edge
	TCCR1B |= (1 << WGM12 | 1 << ICNC1 | 1 << CS11 | 1 << CS10 | 1 << ICES1);
	
	//Setting OCR1AH for a 12 us cycle
	OCR1A = ULTRASONIC_pulse;
	
	//Enabling the Output compare A match and interrupt capture interrupt (Global interrupts still disabled)
	TIMSK1 |= (1 << OCIE1A | 1 << ICIE1);
	
	//Setting the trigger to high
	TRIG_port |= (1 << trigPins[ultrasonicIndex]);
	
	//Resetting the counter to 0
	TCNT1 = 0;
	
	//Enabling interrupts
	sei();
}

//This interrupt is called when the trigger pulse has been turned on for 12us
ISR(TIMER1_COMPA_vect){
	if (OCR1A == ULTRASONIC_pulse)
	{
		//Turning off trigger pins
		TRIG_port &= ~(1 << TRIG1_pin | 1 << TRIG2_pin | 1 << TRIG3_pin);
		
		//Setting OCR1A for timout
		OCR1A = ULTRASONIC_timout;
		
		//Update pulseindex
		pulseIndex = ultrasonicIndex;
	} 
	else if (OCR1A == ULTRASONIC_timout) //Moving on to next dist measurement
	{
		//If no measurement have been made in cycle
		if (pulseIndex == ultrasonicIndex)
		{
			//Saving an erroneous result in dist
			dist[ultrasonicIndex] = 0xFFFF;
			
			//Incrementing the ultrasonicIndex var
			ultrasonicIndex++;
			if(ultrasonicIndex > 2){ultrasonicIndex = 0;}
		}
		
		//Change so interrupt is triggered on rising edge
		TCCR1B |= (1 << ICES1);
		
		//Setting OCR1A for next pulse 
		OCR1A = ULTRASONIC_pulse;
		
		//Setting the trigger to high
		TRIG_port |= (1 << trigPins[ultrasonicIndex]);

		//Resetting the counter to 0
		TCNT1 = 0;
	}
}

//Depending on the DistanceState, this interrupt either reacts on rising or falling edge
ISR(TIMER1_CAPT_vect){	
	//If interrupt is triggered on rising edge and not generating pulse
	if (TCCR1B & (1 << ICES1) && OCR1A == ULTRASONIC_timout)
	{
		//Change so interrupt is triggered on falling edge
		TCCR1B &= ~(1 << ICES1);
		
		risingICR = ICR1;
		
	} else if (!(TCCR1B & (1 << ICES1)) && OCR1A == ULTRASONIC_timout){		
		//Moving the distance measurement to the global var
		dist[ultrasonicIndex] = (ICR1 - risingICR);	
		
		//Incrementing the ultrasonicIndex var
		ultrasonicIndex++;
		if(ultrasonicIndex > 2){ultrasonicIndex = 0;}
	}
}