#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "fsl_pit.h"
#include "clock_config.h"
#include "fsl_uart.h"
#include "fsl_adc16.h"
#include "fsl_tpm.h"
#include "tiempoBotones.h"
#include "rotabit.h"
#include "controlBotones.h"
#include "adctopwm.h"
//#include "delay.h" //Por si necesito un delay

#define DEMO_UART UART1
#define DEMO_UART_CLKSRC BUS_CLK
#define DEMO_UART_CLK_FREQ CLOCK_GetFreq(BUS_CLK)
#define PIT_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)
#define QUARTER_USEC_TO_COUNT(us_4, clockFreqInHz) (uint64_t)((uint64_t)us_4 * clockFreqInHz / 4000000U)


/* Cuidar no dejar en diferente GPIO los LEDs de conteo de canción. En caso
 * de hacerlo, modificar código en donde se cambie de canción. */

#define PUERTO_CANCION_B0   GPIOE  	/* LED A de acuerdo a PDF de requerimientos de la práctica */
#define PIN_CANCION_B0      2U

#define PUERTO_CANCION_B1   GPIOE  	/* LED B de acuerdo a PDF de requerimientos de la práctica */
#define PIN_CANCION_B1      3U

#define PUERTO_BOTON_1      GPIOE  	/* BOTON 1 de acuerdo a PDF de requerimientos de la práctica */
#define PIN_BOTON_1         23U

#define PUERTO_BOTON_2      GPIOE  	/* BOTON 2 de acuerdo a PDF de requerimientos de la práctica */
#define PIN_BOTON_2         21U

#define PUERTO_BOTON_3      GPIOE  	/* BOTON 3 de acuerdo a PDF de requerimientos de la práctica */
#define PIN_BOTON_3         22U


char bufferMain[10]; //Variable para pruebas
volatile uint32_t flagPIT0 = 0;
volatile uint32_t flagPIT1 = 0;
BOTON_DEBOUNCE botonesDebounce[3];
TIPOS_PRESIONADO presionadoBotones[3];
BotonControl botonesControl[3];



void PIT_DriverIRQHandler(void);
void configPits(void);
void configUart(void); //Por si se ocupa en un futuro
void sistemaPrincipal(Port_Rotabit* p);
void reproducirCancion(void);
void evaluarPresionadoBotones(void);
void evaluarAccionBotones(void);

void PIT_DriverIRQHandler(void)
{
	flagPIT0 = PIT_GetStatusFlags(PIT, kPIT_Chnl_0);
	flagPIT1 = PIT_GetStatusFlags(PIT, kPIT_Chnl_1);

	if(flagPIT1)
	{
		PIT_ClearStatusFlags(PIT,1, kPIT_TimerFlag);

		if(counterPush!=0xFFFFFFFF)
		{
			//sprintf(bufferMain, "%d\n",counterPush);
			//PRINTF(bufferMain);
			counterPush++;
		}

	}

	else if(flagPIT0)
	{
		PIT_ClearStatusFlags(PIT,0, kPIT_TimerFlag);
	}


}


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


void sistemaPrincipal(Port_Rotabit* p)
{
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

void reproducirCancion(void)
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

int main(void) {

	adc16_channel_config_t adc16ChannelConfigStruct;

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();

	//char prueba[] ="BIENVENIDO\n\n";
	//configUart();  //UART1
	//UART_WriteBlockingString(UART1, prueba); //UART0 TERMINAL

	configAdc(&adc16ChannelConfigStruct);   // Adc


	Port_Rotabit PR;

	initPortRotabit(&PR, 3); //Numero de leds | Asegurese que los pines de las puerto esten configurados como salidas en LOGICA 1

	SensorPwm s1; // Representa al potenciometro

	initSensorPwm(&s1); //Inicializando variable

	//Inicializando Maquinas de estado de los botones a utilizar
	initBoton(botonesControl);
	initBoton(botonesControl+1); //Pasando la celda 1
	initBoton(botonesControl+2); //Pasando la celda 2

	botonesControl[0].curr_state = DISABLED;
	botonesControl[1].curr_state = DISABLED;
	botonesControl[2].curr_state = DISABLED;


	//Variables para antirebote

	inicializarBotonDebounce(botonesDebounce);
	inicializarBotonDebounce(botonesDebounce+1); //Pasando la celda 1
	inicializarBotonDebounce(botonesDebounce+2); //Pasando la celda 2


	presionadoBotones[0] = NO_ACTION;
	presionadoBotones[1] = NO_ACTION;
	presionadoBotones[2] = NO_ACTION;

	configPits();   //Timer0
	configPwm();

	controlVolumen(&s1, adc16ChannelConfigStruct);

	PRINTF("REPRODUCTOR DE MUSICA INICIADO\n\n");

	while(1) {

		evaluarPresionadoBotones();
		evaluarAccionBotones();

		if(flagPIT0){

			flagPIT0 = 0;
			sistemaPrincipal(&PR);
		}

		reproducirCancion();
		controlVolumen(&s1, adc16ChannelConfigStruct);

	}


	return 0 ;
}
