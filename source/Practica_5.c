#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
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

uint32_t flagPIT0 = 0;
uint32_t flagPIT1 = 0;
char bufferMain[10]; //Variable para pruebas


void PIT_DriverIRQHandler(void);
void configPits(void);
void configUart(void); //Por si se ocupa en un futuro
void sistemaPrincipal(BotonControl* b, GPIO_Type *base, Port_Rotabit* p);
void reproducirCancion(GPIO_Type *base);

void PIT_DriverIRQHandler(void)
{
	flagPIT0 = PIT_GetStatusFlags(PIT, kPIT_Chnl_0);
	flagPIT1 = PIT_GetStatusFlags(PIT, kPIT_Chnl_1);

	if(flagPIT1)
	{
		PIT_ClearStatusFlags(PIT,1, kPIT_TimerFlag);

		if(counterPush!=0xFFFFFFFF)
		{
			sprintf(bufferMain, "%d\n",counterPush);
			PRINTF(bufferMain);
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
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_1, QUARTER_USEC_TO_COUNT(4000U, PIT_SOURCE_CLOCK));

	/* Enable timer interrupts for channel 0 */
	PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
	PIT_EnableInterrupts(PIT, kPIT_Chnl_1, kPIT_TimerInterruptEnable);

	PIT_StopTimer(PIT, kPIT_Chnl_0);
	PIT_StopTimer(PIT, kPIT_Chnl_1);

	EnableIRQ(PIT_IRQn);

	PIT_StartTimer(PIT, kPIT_Chnl_0);

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


void sistemaPrincipal(BotonControl* b, GPIO_Type *base, Port_Rotabit* p)
{
	switch(b->curr_state)
	{

	case PLAY:
		//PRINTF("Reproduciendo Cancion\n");

		if(!rotarInversa)
			rotabitRing(base, p);
		else
			rotabitRingInvert(base, p);

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

void reproducirCancion(GPIO_Type *base)
{
	//char bufer[20];
	//sprintf(bufer, "Esta sonando: %d\n", cancionActual);
	//PRINTF(bufer);

	switch(cancionActual)
	{

	case 0:  //EL TRISTE - JOSE JOSE
		base->PDDR = 0;
		break;

	case 1:
		base->PDDR = PIN16; //Encender pin16 del puerto
		break;

	case 2:
		base->PDDR = PIN17; //Encender pin17 del puerto
		break;

	case 3:
		base->PDDR = PIN16Y17; //Encender pin17 y 16 del puerto
		break;


	default:
		break;
	}

}

int main(void) {

	char prueba[] ="BIENVENIDO\n\n";

	adc16_channel_config_t adc16ChannelConfigStruct;

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();

	//PRINTF("Hello World\n");  //Test UART0 Debug

	configUart();  //UART1
	configAdc(&adc16ChannelConfigStruct);   // Adc
	configPwm(); //PWM

	UART_WriteBlockingString(UART1, prueba); //UART0 TERMINAL

	Port_Rotabit PD;

	initPortRotabit(&PD, 3); //Numero de leds | Asegurese que los pines de las puerto esten configurados como salidas en LOGICA 1

	BotonControl b1, b2, b3;

	SensorPwm s1; // Representa al potenciometro

	initSensorPwm(&s1); //Inicializando variable

	//Inicializando Maquinas de estado de los botones a utilizar
	initBoton(&b1);
	initBoton(&b2);
	initBoton(&b3);

	//Variables para antirebote

	BOTON_DEBOUNCE bd1, bd2, bd3;

	b1.curr_state = DISABLED;
	b2.curr_state = DISABLED;
	b3.curr_state = DISABLED;

	//Inicializando Puerto

	PTD->PDDR = 0;
	PTC->PDDR = 0;

	TIPOS_PRESIONADO presionadoBotones[3];

	presionadoBotones[0] = NO_ACTION;
	presionadoBotones[1] = NO_ACTION;
	presionadoBotones[2] = NO_ACTION;

	configPits();   //Timer0


	while(1) {

		presionadoBotones[0] = maquinaEstadosPush(PTB,0, PTB, 1, PTB, 2, &bd1);
		presionadoBotones[1] = maquinaEstadosPush(PTB,0, PTB, 1, PTB, 2, &bd2);
		presionadoBotones[2] = maquinaEstadosPush(PTB,0, PTB, 1, PTB, 2, &bd3);

		controlBoton1(&b1, presionadoBotones , PTD, &PD);
		controlBoton2(&b2, presionadoBotones , PTD, &PD);
		controlBoton3(&b3, presionadoBotones , PTD, &PD);


		if(flagPIT0){

			flagPIT0 = 0;
			sistemaPrincipal(&b1, PTD, &PD);
			controlVolumen(&s1, adc16ChannelConfigStruct);
		}

		reproducirCancion(PTC);

	}


	return 0 ;
}
