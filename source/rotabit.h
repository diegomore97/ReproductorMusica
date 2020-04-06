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
void rotabitRing(Port_Rotabit* p); //Prototype function
void rotabitRingInvert(Port_Rotabit* p); //Prototype function
void resetRotabit(Port_Rotabit* p); //Prototype function
void avanceCancion(Port_Rotabit* p);

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
	p->maxBits = 4; //MAXIMO 3 BITS
}


void avanceCancion(Port_Rotabit* p)
{
	switch(p->counter){
	case 1:
		GPIO_WritePinOutput(PUERTO_ROTABIT_B0, PIN_ROTABIT_B0 , 0);
		GPIO_WritePinOutput(PUERTO_ROTABIT_B1, PIN_ROTABIT_B1 , 0);
		GPIO_WritePinOutput(PUERTO_ROTABIT_B2, PIN_ROTABIT_B2 , 0);
		break;

	case 2:
		GPIO_WritePinOutput(PUERTO_ROTABIT_B0, PIN_ROTABIT_B0 , 1);
		GPIO_WritePinOutput(PUERTO_ROTABIT_B1, PIN_ROTABIT_B1 , 0);
		GPIO_WritePinOutput(PUERTO_ROTABIT_B2, PIN_ROTABIT_B2 , 0);
		break;

	case 3:
		GPIO_WritePinOutput(PUERTO_ROTABIT_B0, PIN_ROTABIT_B0 , 0);
		GPIO_WritePinOutput(PUERTO_ROTABIT_B1, PIN_ROTABIT_B1 , 1);
		GPIO_WritePinOutput(PUERTO_ROTABIT_B2, PIN_ROTABIT_B2 , 0);
		break;

	case 4:
		GPIO_WritePinOutput(PUERTO_ROTABIT_B0, PIN_ROTABIT_B0 , 0);
		GPIO_WritePinOutput(PUERTO_ROTABIT_B1, PIN_ROTABIT_B1 , 0);
		GPIO_WritePinOutput(PUERTO_ROTABIT_B2, PIN_ROTABIT_B2 , 1);
		break;

	default:
		break;

	}

}


void rotabitRing(Port_Rotabit* p)
{
	switch(p->curr_state)
	{

	case START:

		//PRINTF("ROTABIT: STATE START\n");
		p->counter = 1;
		p->Next_state = ROTATE;

		break;

	case ROTATE:

		//PRINTF("ROTABIT: STATE ROTATE\n");
		p->counter++;

		if (p->counter >= p->maxBits)
			p->Next_state = START;
		else
			p->Next_state = ROTATE;

		break;


	default:
		break;


	}

	p->curr_state = p->Next_state;

	avanceCancion(p);

}

void rotabitRingInvert(Port_Rotabit* p)
{
	switch(p->curr_state)
	{

	case START:

		//PRINTF("ROTABIT INVERTED: STATE START\n");

		p->counter = p->maxBits;
		p->Next_state = ROTATE;

		break;

	case ROTATE:

		//PRINTF("ROTABIT INVERTED: STATE ROTATE\n");

		p->counter--;

		if (p->counter == 1 || p->counter == 0){
			p->Next_state = START;
		}
		else
			p->Next_state = ROTATE;

		break;


	default:
		break;


	}

	p->curr_state = p->Next_state;

	avanceCancion(p);

}


#endif /* ROTABIT_H_ */
