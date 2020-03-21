/*
 * rotabit.h
 *
 *  Created on: 20 mar. 2020
 *      Author: Diego Moreno
 */

#ifndef ROTABIT_H_
#define ROTABIT_H_

typedef enum
{
	START = 1,
	ROTATE,
	FINISH
}ROTABIT_STATES;

typedef struct
{
	uint8_t counter;
	ROTABIT_STATES curr_state;
	ROTABIT_STATES Next_state;
	uint8_t maxBits;

}Port_Rotabit;


void initPortRotabit(Port_Rotabit* p, uint8_t maxBits); //Prototype function

void initPortRotabit(Port_Rotabit* p, uint8_t maxBits)
{
   p->Next_state = START;
   p->curr_state = START;
   p->counter = 0;
   p->maxBits = maxBits;
}


#endif /* ROTABIT_H_ */
