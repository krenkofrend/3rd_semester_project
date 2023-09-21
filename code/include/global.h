#ifndef GLOBAL_H
#define GLOBAL_H

//states for the state machine
typedef enum
{
    state_idle,
    state_pathfind_sound,
    state_pathfind_fire,
    state_extinguish,
    state_evacuate,
    state_shutdown,
}state;
//variabe to keep current state
int current_state;
//counter variables
unsigned long run_path_fire_counter;
unsigned long run_idle_counter;

//motor
//Ports for directions and PWMs
#define DIRECTION_1     PORTB1
#define DIRECTION_2   	PORTB2
#define PWM_1   		PORTD3
#define PWM_2   		PORTB3
//velocity definitions
#define SLOW   		0
#define FAST   		65
#define TURN_SPEED  65
//code cycles to run the corresponding function
#define STOP        2000
#define REVERSE     3000
#define TURN        5000
#define _180_TURN   7500


//ultrasonic
//ports for ultrasonic sensor
#define TRIG_DDR    DDRD
#define TRIG_port   PORTD
#define TRIG1_pin   PORTD5 //right 
#define TRIG2_pin   PORTD6 //front
#define TRIG3_pin   PORTD7 //left
#define ECHO_port   PORTB
#define ECHO_pin    PORTB0
//ultrasonic initiation pulse time
#define ULTRASONIC_pulse    3
//ultrasonic cycle timeout time
#define ULTRASONIC_timout   0xFFF0
//range definitions
#define F_RANGE   	        2000 //range for the front ultrasonic sensor 
#define S_RANGE   	        1900 //range for the side ultrasonic sensors 
#define TOO_CLOSE_RANGE   	1200//range for big turn 


//photo transistors
//4 right, 5 left, 1 center, 2 top, 3 bottom
#define Out45   PORTB5
#define Out23   PORTD4
#define Out1    PORTB4
//ADC channels
#define in35    1
#define in124   0
//fire detection threshold
#define FIRE_U 575 
#define FIRE_D 550 


//microphones
#define VOLTAGE 2600 //voltage threshold in mV
#define ADC_PIN 0 //ADC channel 
#define V_REF   5000 //reference voltage 5000mV 


//PUMP
#define PUMP PORTD2


#endif

