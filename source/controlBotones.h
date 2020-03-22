/*
 * controlBotones.h
 *
 *  Created on: 21 mar. 2020
 *      Author: Diego Moreno
 */

#ifndef CONTROLBOTONES_H_
#define CONTROLBOTONES_H_

typedef enum
{
	STOP = 1,
	PLAY,
	PAUSE
}BOTON1_STATES;

typedef enum
{
	NEXT = 1,
	FWD
}BOTON2_STATES;

typedef enum
{
	PREW = 1,
	BWD
}BOTON3_STATES;

typedef struct
{
	uint8_t curr_state;
	uint8_t Next_state;

}BotonControl;


void controlBotones(BotonControl* b); //Prototype function
void controlBoton1(BotonControl* b, GPIO_Type *base, Port_Rotabit* p); //Prototype Function
void controlBoton2(BotonControl* b, GPIO_Type *base,Port_Rotabit* p); //Prototype Function
void controlBoton3(BotonControl* b, GPIO_Type *base,Port_Rotabit* p); //Prototype Function

void initBoton(BotonControl* b)
{
	b->Next_state = 1; //NO ES NUMERO MAGICO MIS ESTADOS SIEMPRE COMIENZAN EN 1
	b->curr_state = 1; //NO ES NUMERO MAGICO MIS ESTADOS SIEMPRE COMIENZAN EN 1
}

void controlBoton1(BotonControl* b, GPIO_Type *base, Port_Rotabit* p)
{
	initPortRotabit(p, 3);

	switch(b->curr_state)
	{

	case STOP:
		base->PDDR = 0; //STOP
		b->Next_state = PLAY;
		break;

	case PLAY:
		b->Next_state = PAUSE;
		break;

	case PAUSE:
		b->Next_state = PLAY;
		break;

	default:
		break;
	}

	b->curr_state = b->Next_state;

}

void controlBoton2(BotonControl* b, GPIO_Type *base, Port_Rotabit* p)
{

	switch(b->curr_state)
	{

	case NEXT:
		break;

	case FWD:
		break;


	default:
		break;
	}
}

void controlBoton3(BotonControl* b, GPIO_Type *base, Port_Rotabit* p)
{

	switch(b->curr_state)
	{

	case PREW:

		break;

	case BWD:
		initPortRotabit(p, 3);
		rotabitRingInvert(base, p);
		break;

	default:
		break;
	}
}


#endif /* CONTROLBOTONES_H_ */
