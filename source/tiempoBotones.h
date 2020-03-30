/*
 * tiempoBotones.h
 *
 *  Created on: 24 mar. 2020
 *      Author: Diego Moreno
 */

#ifndef TIEMPOBOTONES_H_
#define TIEMPOBOTONES_H_

#define thousandMiliseconds	7U //1 segundo
#define fiftyMiliseconds	2U //50 milisegundos


typedef enum
{
	DISABLED,
	COUNT_EN,
	ENABLED,
	COUNT_DIS
} ESTADOS_PUSH;


typedef enum
{
	NORMAL,
	PROLONGADO,
	PROLONGADO_RELEASE,
	NO_ACTION
} TIPOS_PRESIONADO;

typedef struct
{
	ESTADOS_PUSH estadoPushActual;
}BOTON_DEBOUNCE;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

TIPOS_PRESIONADO maquinaEstadosPush(GPIO_Type *base, uint32_t pinLeer, BOTON_DEBOUNCE* bd);
void maquinaEstadosReproductor(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

volatile bool pitIsrFlag = false;
uint32_t conteoMuestreo = 0U;
uint32_t counterPush = 0U;
ESTADOS_PUSH estadoPushSiguiente = DISABLED;

TIPOS_PRESIONADO maquinaEstadosPush(GPIO_Type *base, uint32_t pinLeer, BOTON_DEBOUNCE* bd)
{
	static uint32_t diffCounterPush = 0;
	TIPOS_PRESIONADO valorRetorno = NO_ACTION;

	switch(bd->estadoPushActual)
	{
	case DISABLED:

		if(!(GPIO_ReadPinInput(base , pinLeer)))
		{
			//PRINTF("DISABLED PIN 1!\n");
			estadoPushSiguiente=COUNT_EN;
			PIT_StartTimer(PIT, kPIT_Chnl_1);
		}
		else
		{
			estadoPushSiguiente=DISABLED;
		}
		break;

	case COUNT_EN:
		//PRINTF("COUNT_EN PIN 1!\n");
		if(GPIO_ReadPinInput(base , pinLeer))
		{
			estadoPushSiguiente=DISABLED;
			PIT_StopTimer(PIT,kPIT_Chnl_1);
			counterPush=0;
		}
		else
		{
			if(counterPush>=fiftyMiliseconds)
			{
				estadoPushSiguiente=ENABLED;
			}
			else
			{
				estadoPushSiguiente=COUNT_EN;
			}
		}
		break;
	case ENABLED:
		//PRINTF("ENABLED PIN 1!\n");
		if(GPIO_ReadPinInput(base , pinLeer))
		{
			estadoPushSiguiente=COUNT_DIS;
			diffCounterPush=counterPush;
			if(counterPush>thousandMiliseconds)
			{
				valorRetorno = PROLONGADO;
			}
			else
			{

			}
		}
		else
		{
			estadoPushSiguiente=ENABLED;
			if(counterPush>thousandMiliseconds)
			{
				valorRetorno = PROLONGADO;
			}
			else
			{

			}
		}
		break;
	case COUNT_DIS:
		if(!(GPIO_ReadPinInput(base , pinLeer)))
		{
			estadoPushSiguiente=ENABLED;
			if(diffCounterPush>thousandMiliseconds)
			{
				valorRetorno = PROLONGADO;
			}
			else
			{

			}
		}
		else
		{
			if((counterPush-diffCounterPush)>=fiftyMiliseconds)
			{
				estadoPushSiguiente=DISABLED;
				PIT_StopTimer(PIT,kPIT_Chnl_1);
				counterPush=0;
				if(diffCounterPush<=thousandMiliseconds)
				{
					valorRetorno = NORMAL;
				}
				else
				{
					valorRetorno = PROLONGADO_RELEASE;
				}
			}
			else
			{
				estadoPushSiguiente=COUNT_DIS;
				if(diffCounterPush>thousandMiliseconds)
				{
					valorRetorno = PROLONGADO;
				}
				else
				{

				}
			}
		}
		break;

	default:
		break;
	}

	bd->estadoPushActual=estadoPushSiguiente;

	return valorRetorno;

}

#endif /* TIEMPOBOTONES_H_ */
