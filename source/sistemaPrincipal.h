/*
 * sistemaPrincipal.h
 *
 *  Created on: 5 abr. 2020
 *      Author: Diego Moreno
 */

#ifndef SISTEMAPRINCIPAL_H_
#define SISTEMAPRINCIPAL_H_

#include "tiempoBotones.h"
#include "controlBotones.h"
#include "rotabit.h"
#include "adctopwm.h"
//#include "delay.h" //Por si necesito un delay

#define DEMO_UART UART1
#define DEMO_UART_CLKSRC BUS_CLK
#define DEMO_UART_CLK_FREQ CLOCK_GetFreq(BUS_CLK)
#define PIT_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)
#define QUARTER_USEC_TO_COUNT(us_4, clockFreqInHz) (uint64_t)((uint64_t)us_4 * clockFreqInHz / 4000000U)


char bufferMain[10]; //Variable para pruebas
volatile uint32_t flagPIT0 = 0;
volatile uint32_t flagPIT1 = 0;
BOTON_DEBOUNCE botonesDebounce[3];
TIPOS_PRESIONADO presionadoBotones[3];
BotonControl botonesControl[3];


void configPits(void);
void configUart(void); //Por si se ocupa en un futuro
void visualizarAvanceCancion(Port_Rotabit* p);
void visualizarCancionActual(void);
void evaluarPresionadoBotones(void);
void evaluarAccionBotones(void);


void configPits(void)
{
	pit_config_t My_PIT;

	PIT_GetDefaultConfig(&My_PIT);

	PIT_Init(PIT, &My_PIT);

	/* Set timer period for channel 0 */
	//PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, QUARTER_USEC_TO_COUNT(90U, PIT_SOURCE_CLOCK));
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_0,MSEC_TO_COUNT(500, PIT_CLK_SRC_HZ_HP));
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_1, USEC_TO_COUNT(50000U, PIT_SOURCE_CLOCK));

	/* Enable timer interrupts for channel 0 */
	PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
	PIT_EnableInterrupts(PIT, kPIT_Chnl_1, kPIT_TimerInterruptEnable);

	PIT_StopTimer(PIT, kPIT_Chnl_0);
	PIT_StopTimer(PIT, kPIT_Chnl_1);

	EnableIRQ(PIT_IRQn);

}

void configUart(void)
{
	uart_config_t config;

	/*
	 * config.baudRate_Bps = 115200U;
	 * config.parityMode = kUART_ParityDisabled;
	 * config.stopBitCount = kUART_OneStopBit;
	 * config.txFifoWatermark = 0;
	 * config.rxFifoWatermark = 1;
	 * config.enableTx = false;
	 * config.enableRx = false;
	 */
	UART_GetDefaultConfig(&config);
	config.enableTx = true;
	config.enableRx = true;

	UART_Init(UART1, &config, DEMO_UART_CLK_FREQ);

}


void visualizarAvanceCancion(Port_Rotabit* p)
{
	if(flagPIT0){

		flagPIT0 = 0;

	switch(botonesControl[0].curr_state)
	{

	case PLAY:
		//PRINTF("Reproduciendo Cancion\n");

		if(!atrasar)
			rotabitRing(p);
		else
			rotabitRingInvert(p);

		break;

	case PAUSE:
		//PRINTF("Cancion Pausada\n");
		break;

	case STOP:
		//PRINTF("Sistema detenido\n");
		break;

	default:
		break;
	}

	}
}

void visualizarCancionActual(void)
{
	//char bufer[20];
	//sprintf(bufer, "Esta sonando: %d\n", cancionActual);
	//PRINTF(bufer);

	switch(cancionActual)
	{

	case 0:  //EL TRISTE - JOSE JOSE
		GPIO_WritePinOutput(PUERTO_CANCION_B0, PIN_CANCION_B0 , 0);
		GPIO_WritePinOutput(PUERTO_CANCION_B1, PIN_CANCION_B1 , 0);
		break;

	case 1:
		GPIO_WritePinOutput(PUERTO_CANCION_B0, PIN_CANCION_B0 , 1);
		GPIO_WritePinOutput(PUERTO_CANCION_B1, PIN_CANCION_B1 , 0);
		break;

	case 2:
		GPIO_WritePinOutput(PUERTO_CANCION_B0, PIN_CANCION_B0 , 0);
		GPIO_WritePinOutput(PUERTO_CANCION_B1, PIN_CANCION_B1 , 1);
		break;

	case 3:
		GPIO_WritePinOutput(PUERTO_CANCION_B0, PIN_CANCION_B0 , 1);
		GPIO_WritePinOutput(PUERTO_CANCION_B1, PIN_CANCION_B1 , 1);
		break;


	default:
		break;
	}

}

void evaluarPresionadoBotones(void)
{
	presionadoBotones[0] = maquinaEstadosPush(PUERTO_BOTON_1, PIN_BOTON_1, botonesDebounce);
	presionadoBotones[1] = maquinaEstadosPush(PUERTO_BOTON_2, PIN_BOTON_2, botonesDebounce+1); //Pasando la celda 1
	presionadoBotones[2] = maquinaEstadosPush(PUERTO_BOTON_3, PIN_BOTON_3, botonesDebounce+2); //Pasando la celda 2
}

void evaluarAccionBotones(void)
{
	controlBoton1(botonesControl,   presionadoBotones);
	controlBoton2(botonesControl+1, presionadoBotones); //Pasando la celda 1
	controlBoton3(botonesControl+2, presionadoBotones); //Pasando la celda 2

}



#endif /* SISTEMAPRINCIPAL_H_ */
