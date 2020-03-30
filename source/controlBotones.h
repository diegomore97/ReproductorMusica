/*
 * controlBotones.h
 *
 *  Created on: 21 mar. 2020
 *      Author: Diego Moreno
 */

#ifndef CONTROLBOTONES_H_
#define CONTROLBOTONES_H_

#define MAX_CANCIONES 3
#define PIT_CLK_SRC_HZ_HP ((uint64_t)24000000)
uint32_t PIN16 = 65536;
uint32_t PIN17 = 131072;
uint32_t PIN16Y17 = 196608;

volatile int cancionActual = 0;
uint8_t atrasar = 0;


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
void controlBoton1(BotonControl* b, TIPOS_PRESIONADO* TP, GPIO_Type *base, Port_Rotabit* p); //Prototype Function
void controlBoton2(BotonControl* b, TIPOS_PRESIONADO* TP, GPIO_Type *base,Port_Rotabit* p); //Prototype Function
void controlBoton3(BotonControl* b, TIPOS_PRESIONADO* TP, GPIO_Type *base,Port_Rotabit* p); //Prototype Function

void initBoton(BotonControl* b)
{
	b->Next_state = 1; //NO ES NUMERO MAGICO MIS ESTADOS SIEMPRE COMIENZAN EN 1
	b->curr_state = 1; //NO ES NUMERO MAGICO MIS ESTADOS SIEMPRE COMIENZAN EN 1
}

void controlBoton1(BotonControl* b,TIPOS_PRESIONADO* TP, GPIO_Type *base, Port_Rotabit* p)
{

	switch(b->curr_state)
	{

	case STOP:

		if(TP[0] == NORMAL)
		{
			b->Next_state = PLAY;
		}
		else if(TP[0] == PROLONGADO_RELEASE)
		{
			b->Next_state = STOP;
		}

		else
		{
			cancionActual = 0;
			initBoton(b);
			base->PDDR = 0; //APAGAR LEDS
			resetRotabit(p);
			b->Next_state = STOP;
		}

		break;

	case PLAY:

		if(TP[0] == NORMAL)
		{
			b->Next_state = PAUSE;
		}
		else if(TP[0] == PROLONGADO_RELEASE)
		{
			b->Next_state = STOP;
		}

		else
		{
			b->Next_state = PLAY;
			//PRINTF("PLAY\n");
		}


		break;

	case PAUSE:
		if(TP[0] == NORMAL)
		{
			b->Next_state = PLAY;
		}
		else if(TP[0] == PROLONGADO_RELEASE)
		{
			b->Next_state = STOP;

		}
		else
		{
			//PIT_StopTimer(PIT,kPIT_Chnl_0);
			b->Next_state = PAUSE;
		}
		break;

	default:
		break;
	}

	b->curr_state = b->Next_state;

}

void controlBoton2(BotonControl* b, TIPOS_PRESIONADO* TP, GPIO_Type *base, Port_Rotabit* p)
{

	switch(b->curr_state)
	{

	case NEXT:

		if(TP[1] == NORMAL)
		{
			PRINTF("SIGUIENTE CANCION\n");
			cancionActual+=1;

			if(cancionActual > MAX_CANCIONES)
			{
				cancionActual = 0;
			}

		}

		else if(TP[1] == PROLONGADO)
		{
			PIT_SetTimerPeriod(PIT, kPIT_Chnl_0,MSEC_TO_COUNT(100, PIT_CLK_SRC_HZ_HP));
			b->Next_state = FWD;
		}

		else
		{
			b->Next_state = NEXT;
		}
		break;

	case FWD:
		if(TP[1] == PROLONGADO)
		{
			//PRINTF("ADELANTANDO CANCION\n");
			b->Next_state = FWD;
		}

		else
		{
			//PIT_SetTimerPeriod(PIT, kPIT_Chnl_0,MSEC_TO_COUNT(500, PIT_CLK_SRC_HZ_HP));
			b->Next_state = NEXT;
		}

		break;


	default:
		break;
	}

	b->curr_state = b->Next_state;
}

void controlBoton3(BotonControl* b, TIPOS_PRESIONADO* TP, GPIO_Type *base, Port_Rotabit* p)
{

	switch(b->curr_state)
	{

	case PREW:

		if(TP[2] == NORMAL)
		{
			PRINTF("CANCION ANTERIOR\n");
			cancionActual-=1;

			if(cancionActual < 0)
			{
				cancionActual = 0;
			}

		}

		else if(TP[2] == PROLONGADO)
		{
			PIT_SetTimerPeriod(PIT, kPIT_Chnl_0,MSEC_TO_COUNT(1000, PIT_CLK_SRC_HZ_HP));
			atrasar = 1;
			b->Next_state = BWD;
		}

		else
		{
			b->Next_state = PREW;
		}

		break;

	case BWD:

		if(TP[2] == PROLONGADO)
		{
			//PRINTF("ATRASANDO CANCION\n");
			b->Next_state = BWD;
		}

		else
		{
			//atrasar = 0;
			//PIT_SetTimerPeriod(PIT, kPIT_Chnl_0,MSEC_TO_COUNT(500, PIT_CLK_SRC_HZ_HP));
			b->Next_state = PREW;
		}

		break;

	default:
		break;
	}

	b->curr_state = b->Next_state;
}


#endif /* CONTROLBOTONES_H_ */
