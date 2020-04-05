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

volatile uint8_t cancionActual = 0;
uint8_t atrasar = 0;
uint8_t adelantar = 0;

void cambiarPeriodo(void);


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
void controlBoton1(BotonControl* b, TIPOS_PRESIONADO* TP); //Prototype Function
void controlBoton2(BotonControl* b, TIPOS_PRESIONADO* TP); //Prototype Function
void controlBoton3(BotonControl* b, TIPOS_PRESIONADO* TP); //Prototype Function

void initBoton(BotonControl* b)
{
	b->Next_state = 1; //NO ES NUMERO MAGICO MIS ESTADOS SIEMPRE COMIENZAN EN 1
	b->curr_state = 1; //NO ES NUMERO MAGICO MIS ESTADOS SIEMPRE COMIENZAN EN 1
}

void cambiarPeriodo(void)
{
	if(adelantar || atrasar){
		PIT_SetTimerPeriod(PIT, kPIT_Chnl_0,MSEC_TO_COUNT(100, PIT_CLK_SRC_HZ_HP));
		//PRINTF("PERIODO A 100 MS\n");
	}
	else{
		PIT_SetTimerPeriod(PIT, kPIT_Chnl_0,MSEC_TO_COUNT(500, PIT_CLK_SRC_HZ_HP));
		//PRINTF("PERIODO A 500 MS\n");
	}

}

void controlBoton1(BotonControl* b, TIPOS_PRESIONADO* TP)
{

	switch(b->curr_state)
	{

	case STOP:

		if(TP[0] == NORMAL)
		{
			PIT_StartTimer(PIT,kPIT_Chnl_0);
			b->Next_state = PLAY;
		}
		else if(TP[0] == PROLONGADO_RELEASE)
		{
			b->Next_state = STOP;
		}

		else
		{
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
			cancionActual = 0;
			initBoton(b);
			b->Next_state = STOP;
		}

		else
		{
			b->Next_state = PLAY;
			//PRINTF("PLAY\n");
		}


		break;

	case PAUSE:

		PIT_StopTimer(PIT,kPIT_Chnl_0);

		if(TP[0] == NORMAL)
		{
			PIT_StartTimer(PIT,kPIT_Chnl_0);
			b->Next_state = PLAY;
		}

		else if(TP[0] == PROLONGADO_RELEASE)
		{
			cancionActual = 0;
			initBoton(b);
			b->Next_state = STOP;

		}

		else
		{
			b->Next_state = PAUSE;
		}
		break;

	default:
		break;
	}

	b->curr_state = b->Next_state;

}

void controlBoton2(BotonControl* b, TIPOS_PRESIONADO* TP)
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
			adelantar = 1;
			cambiarPeriodo();
			b->Next_state = FWD;
		}

		else
		{
			adelantar = 0;
			cambiarPeriodo();
			b->Next_state = NEXT;
		}

		break;


	default:
		break;
	}

	b->curr_state = b->Next_state;
}

void controlBoton3(BotonControl* b, TIPOS_PRESIONADO* TP)
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
			atrasar = 1;
			cambiarPeriodo();
			b->Next_state = BWD;
		}

		else
		{
			atrasar = 0;
			cambiarPeriodo();
			b->Next_state = PREW;
		}

		break;

	default:
		break;
	}

	b->curr_state = b->Next_state;
}


#endif /* CONTROLBOTONES_H_ */
