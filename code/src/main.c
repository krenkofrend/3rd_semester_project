//Standard includes
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>
#include <util/delay.h>
#include "global.h"
#define F_CPU 16000000UL

int main(void){
	//Call initialization functions in other files so all pins are settup 
    idle_start();
    //pathfind_sound_start();
    pathfind_fire_start();
    //Start cycle counters
    run_idle_counter = 0;
    run_path_fire_counter = 0;
    //Set the initial state
    current_state = state_idle;
    while (1){
        //Define the state machine
        switch(current_state){
            case state_idle:
                idle();
                break;
            case state_pathfind_fire:
                pathfind_fire();
                break;
            case state_extinguish:
                extinguish();
                break;
            case state_evacuate:
                evacuate();
                break;
            case state_shutdown:
                break;
        }
    }
}

