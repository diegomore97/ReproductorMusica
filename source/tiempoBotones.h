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
	COUNT_EN0,
	ENABLED0,
	COUNT_DIS0,
	COUNT_EN1,
	ENABLED1,
	COUNT_DIS1,
	COUNT_EN2,
	ENABLED2,
	COUNT_DIS2
} ESTADOS_PUSH;


typedef enum
{
	PPS_NORMAL,
	PPS_PROLONGADO_RELEASE,
	NF_NORMAL,
	NF_PROLONGADO,
	NF_PROLONGADO_RELEASE,
	PB_NORMAL,
	PB_PROLONGADO,
	PB_PROLONGADO_RELEASE,
	NO_ACTION

} TIPOS_PRESIONADO;

typedef struct
{
	ESTADOS_PUSH estadoPushActual;
}BOTON_DEBOUNCE;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

TIPOS_PRESIONADO maquinaEstadosPush(GPIO_Type *base1, uint32_t pinLeer1,GPIO_Type *base2, uint32_t pinLeer2,GPIO_Type *base3, uint32_t pinLeer3, BOTON_DEBOUNCE* bd);
void maquinaEstadosReproductor(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

volatile bool pitIsrFlag = false;
uint32_t conteoMuestreo = 0U;
uint32_t counterPush = 0U;
uint16_t valorADC = 0;
int8_t	 baseSuma = 1;
uint8_t numCancion = 0U;
ESTADOS_PUSH estadoPushSiguiente = DISABLED;

TIPOS_PRESIONADO maquinaEstadosPush(GPIO_Type *base1, uint32_t pinLeer1,GPIO_Type *base2, uint32_t pinLeer2,GPIO_Type *base3, uint32_t pinLeer3, BOTON_DEBOUNCE* bd)
{
	static uint32_t diffCounterPush = 0;
	TIPOS_PRESIONADO valorRetorno = NO_ACTION;

	switch(bd->estadoPushActual)
	{
	case DISABLED:
		if(!(GPIO_ReadPinInput(base1 , pinLeer1)))
		{
			//PRINTF("DISABLED PIN 1!\n");
			estadoPushSiguiente=COUNT_EN0;
			PIT_StartTimer(PIT, kPIT_Chnl_1);
		}
		else if(!(GPIO_ReadPinInput(base2, pinLeer2)))
		{
			//PRINTF("DISABLED PIN 2!\n");
			estadoPushSiguiente=COUNT_EN1;
			PIT_StartTimer(PIT, kPIT_Chnl_1);
		}
		else if(!(GPIO_ReadPinInput(base3, pinLeer3)))
		{
			//PRINTF("DISABLED PIN 3!\n");
			estadoPushSiguiente=COUNT_EN2;
			PIT_StartTimer(PIT, kPIT_Chnl_1);
		}
		else
		{
			estadoPushSiguiente=DISABLED;
		}
		break;
	case COUNT_EN0:
		//PRINTF("COUNT_EN PIN 1!\n");
		if(GPIO_ReadPinInput(base1 , pinLeer1))
		{
			estadoPushSiguiente=DISABLED;
			PIT_StopTimer(PIT,kPIT_Chnl_1);
			counterPush=0;
		}
		else
		{
			if(counterPush>=fiftyMiliseconds)
			{
				estadoPushSiguiente=ENABLED0;
			}
			else
			{
				estadoPushSiguiente=COUNT_EN0;
			}
		}
		break;
	case ENABLED0:
		//PRINTF("ENABLED PIN 1!\n");
		if(GPIO_ReadPinInput(base1 , pinLeer1))
		{
			estadoPushSiguiente=COUNT_DIS0;
			diffCounterPush=counterPush;
			if(counterPush>thousandMiliseconds)
			{
				valorRetorno = PB_PROLONGADO;
			}
			else
			{

			}
		}
		else
		{
			estadoPushSiguiente=ENABLED0;
			if(counterPush>thousandMiliseconds)
			{
				valorRetorno = PB_PROLONGADO;
			}
			else
			{

			}
		}
		break;
	case COUNT_DIS0:
		if(!(GPIO_ReadPinInput(base1 , pinLeer1)))
		{
			estadoPushSiguiente=ENABLED0;
			if(diffCounterPush>thousandMiliseconds)
			{
				valorRetorno = PB_PROLONGADO;
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
					valorRetorno = PB_NORMAL;
				}
				else
				{
					valorRetorno = PB_PROLONGADO_RELEASE;
				}
			}
			else
			{
				estadoPushSiguiente=COUNT_DIS0;
				if(diffCounterPush>thousandMiliseconds)
				{
					valorRetorno = PB_PROLONGADO;
				}
				else
				{

				}
			}
		}
		break;
	case COUNT_EN1:
		if(GPIO_ReadPinInput(base2 , pinLeer2))
		{
			estadoPushSiguiente=DISABLED;
			PIT_StopTimer(PIT,kPIT_Chnl_1);
			counterPush=0;
		}
		else
		{
			if(counterPush>=fiftyMiliseconds)
			{
				estadoPushSiguiente=ENABLED1;
			}
			else
			{
				estadoPushSiguiente=COUNT_EN1;
			}
		}
		break;
	case ENABLED1:
		if(GPIO_ReadPinInput(base2 , pinLeer2))
		{
			estadoPushSiguiente=COUNT_DIS1;
			diffCounterPush=counterPush;
		}
		else
		{
			estadoPushSiguiente=ENABLED1;
		}
		break;
	case COUNT_DIS1:
		if(!(GPIO_ReadPinInput(base2 , pinLeer2)))
		{
			estadoPushSiguiente=ENABLED1;
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
					valorRetorno = PPS_NORMAL;
				}
				else
				{
					valorRetorno = PPS_PROLONGADO_RELEASE;
				}
			}
			else
			{
				estadoPushSiguiente=COUNT_DIS1;
			}
		}
		break;
	case COUNT_EN2:
		if(GPIO_ReadPinInput(base3 , pinLeer3))
		{
			estadoPushSiguiente=DISABLED;
			PIT_StopTimer(PIT,kPIT_Chnl_1);
			counterPush=0;
		}
		else
		{
			if(counterPush>=fiftyMiliseconds)
			{
				estadoPushSiguiente=ENABLED2;
			}
			else
			{
				estadoPushSiguiente=COUNT_EN2;
			}
		}
		break;
	case ENABLED2:
		if(GPIO_ReadPinInput(base3 , pinLeer3))
		{
			estadoPushSiguiente=COUNT_DIS2;
			diffCounterPush=counterPush;
			if(counterPush>thousandMiliseconds)
			{
				valorRetorno = NF_PROLONGADO;
			}
			else
			{

			}
		}
		else
		{
			estadoPushSiguiente=ENABLED2;
			if(counterPush>thousandMiliseconds)
			{
				valorRetorno = NF_PROLONGADO;
			}
			else
			{

			}
		}
		break;
	case COUNT_DIS2:
		if(!(GPIO_ReadPinInput(base3 , pinLeer3)))
		{
			estadoPushSiguiente=ENABLED2;
			if(diffCounterPush>thousandMiliseconds)
			{
				valorRetorno = NF_PROLONGADO;
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
					valorRetorno = NF_NORMAL;
				}
				else
				{
					valorRetorno = NF_PROLONGADO_RELEASE;
				}
			}
			else
			{
				estadoPushSiguiente=COUNT_DIS2;
				if(diffCounterPush>thousandMiliseconds)
				{
					valorRetorno = NF_PROLONGADO;
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
