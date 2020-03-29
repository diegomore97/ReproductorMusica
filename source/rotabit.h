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
	ROTATE
}ROTABIT_STATES;

typedef struct
{
	uint8_t counter;
	ROTABIT_STATES curr_state;
	ROTABIT_STATES Next_state;
	uint8_t maxBits; //MAXIMO 8 BITS

}Port_Rotabit;


void initPortRotabit(Port_Rotabit* p, uint8_t maxBits); //Prototype function
void rotabitRing(GPIO_Type *base, Port_Rotabit* p); //Prototype function
void rotabitRingInvert(GPIO_Type *base, Port_Rotabit* p); //Prototype function
uint8_t getBits(uint8_t maxBits); //Prototype function
void resetRotabit(Port_Rotabit* p); //Prototype function

void resetRotabit(Port_Rotabit* p)
{
	p->Next_state = START;
	p->curr_state = START;
	p->counter = 1;
}

void initPortRotabit(Port_Rotabit* p, uint8_t maxBits)
{
	p->Next_state = START;
	p->curr_state = START;
	p->counter = 1;
	p->maxBits = getBits(maxBits); //MAXIMO 8 BITS
}

uint8_t getBits(uint8_t maxBits)
{
	uint8_t bits;

	switch(maxBits)
	{
	case 0:
		bits = 0;
		break;

	case 1:
		bits = 1;
		break;

	case 2:
		bits = 2;
		break;

	case 3:
		bits = 4;
		break;

	case 4:
		bits = 8;
		break;

	case 5:
		bits = 16;
		break;

	case 6:
		bits = 32;
		break;

	case 7:
		bits = 64;
		break;

	case 8:
		bits = 128;
		break;

	default:
		break;
	}

	return bits;

}

void rotabitRing(GPIO_Type *base, Port_Rotabit* p)
{
	switch(p->curr_state)
	{

	case START:

		//PRINTF("ROTABIT: STATE START\n");
		if((!p->counter) || (p->counter >= p->maxBits))
			p->counter = 1;

		p->Next_state = ROTATE;

		break;

	case ROTATE:

		//PRINTF("ROTABIT: STATE ROTATE\n");
		p->counter = p->counter << 1;
		p->Next_state = ROTATE;

		if (p->counter >= p->maxBits)
			p->Next_state = START;

		break;


	default:
		break;


	}

	p->curr_state = p->Next_state;
	base->PDDR = p->counter;

}

void rotabitRingInvert(GPIO_Type *base, Port_Rotabit* p)
{
	switch(p->curr_state)
	{

	case START:

		//PRINTF("ROTABIT INVERTED: STATE START\n");

		if (p->counter == 1){
			p->counter = p->maxBits;
		}

		p->Next_state = ROTATE;

		break;

	case ROTATE:

		//PRINTF("ROTABIT INVERTED: STATE ROTATE\n");

		p->counter = p->counter >> 1;
		p->Next_state = ROTATE;

		if (p->counter == 1){
			p->Next_state = START;
		}


		break;


	default:
		break;


	}

	p->curr_state = p->Next_state;
	base->PDDR = p->counter;

}


#endif /* ROTABIT_H_ */
