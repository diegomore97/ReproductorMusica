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
#include "debounce.h"
#include "rotabit.h"
#include "controlBotones.h"
#include "adctopwm.h"
//#include "delay.h" //Por si necesito un delay


#define PIT_CLK_SRC_HZ_HP ((uint64_t)24000000)

#define DEMO_UART UART1
#define DEMO_UART_CLKSRC BUS_CLK
#define DEMO_UART_CLK_FREQ CLOCK_GetFreq(BUS_CLK)

uint32_t flagPIT0 = 0;
uint32_t flagPIT1 = 0;


void PIT_DriverIRQHandler(void);
void configPit(void);
void configPit_2(void);
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
	}

	else if(flagPIT0)
	{
		PIT_ClearStatusFlags(PIT,0, kPIT_TimerFlag);
	}

}


void configPit(void)
{
	pit_config_t My_PIT;

	PIT_GetDefaultConfig(&My_PIT);

	PIT_Init(PIT, &My_PIT);

	PIT_SetTimerPeriod(PIT, kPIT_Chnl_0,MSEC_TO_COUNT(500, PIT_CLK_SRC_HZ_HP));

	PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable );

	PIT_StopTimer(PIT, kPIT_Chnl_0);

	EnableIRQ(PIT_IRQn);

	PIT_StartTimer(PIT, kPIT_Chnl_0);

}

void configPit_2(void)
{
	pit_config_t My_PIT_2;

	PIT_GetDefaultConfig(&My_PIT_2);

	PIT_Init(PIT, &My_PIT_2);

	PIT_SetTimerPeriod(PIT, kPIT_Chnl_1,MSEC_TO_COUNT(100, PIT_CLK_SRC_HZ_HP));

	PIT_EnableInterrupts(PIT, kPIT_Chnl_1, kPIT_TimerInterruptEnable );

	PIT_StopTimer(PIT, kPIT_Chnl_1);

	EnableIRQ(PIT_IRQn);

	PIT_StartTimer(PIT, kPIT_Chnl_1);

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
		rotabitRing(base, p);
		break;

	case PAUSE:
		//PRINTF("Cancion Pausada\n");
		break;

	case STOP:
		//PRINTF("Sistema detenido\n");
		initBoton(b);
		base->PDDR = 0; //STOP
		break;

	default:
		break;
	}


}

void reproducirCancion(GPIO_Type *base)
{
	char bufer[20];
	sprintf(bufer, "Esta sonando: %d\n", cancionActual);
	PRINTF(bufer);

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
	configPit();   //Timer0
	configPit_2(); //Timer0 for ADC
	configAdc(&adc16ChannelConfigStruct);   // Adc
	configPwm(); //PWM

	UART_WriteBlockingString(UART1, prueba); //UART0 TERMINAL

	PinDebounce pb0, pb1, pb2; //Variable que guardara la configuracion para la maquina de estados de ese boton PTB1
	Port_Rotabit PD;

	//BOTON PLAY | PAUSE | STOP
	initPinDebounce(&pb0 , 1, 2); //Inicializar configuracion | Parametros 1 y 3 relacionados con la velocidad de pulsado

	//BOTON NEXT | FWD
	initPinDebounce(&pb1 , 1, 2);

	//BOTON PREW | BWD
	initPinDebounce(&pb2 , 1, 2);

	initPortRotabit(&PD, 3); //Numero de leds | Asegurese que los pines de las puerto esten configurados como salidas en LOGICA 1

	BotonControl b1, b2, b3;

	SensorPwm s1; // Representa al potenciometro

	initSensorPwm(&s1); //Inicializando variable

	//Inicializando Maquinas de estado de los botones a utilizar
	initBoton(&b1);
	initBoton(&b2);
	initBoton(&b3);

	//Inicializando Puerto

	PTD->PDDR = 0;
	PTC->PDDR = 0;


	while(1) {

		if(!(GPIO_ReadPinInput(PTB, 0) ) ){  //PTB0 recibio un pulso en bajo

			antiBounceButtonPullUp(PTB, 0, &pb0); //Ya termino el antirrebote?
			//PRINTF("BOTON 1 PRESIONADO\n");
			controlBoton1(&b1, PTB, &PD);

		}

		else if(!(GPIO_ReadPinInput(PTB , 1) ) ){  //PTB1 recibio un pulso en bajo

			antiBounceButtonPullUp(PTB, 1, &pb1); //Ya termino el antirrebote?
			//PRINTF("BOTON 2 PRESIONADO\n");
			controlBoton2(&b2, PTB, &PD);
		}


		else if(!(GPIO_ReadPinInput(PTB , 2) ) ){ //PTB2 recibio un pulso en bajo

			antiBounceButtonPullUp(PTB, 2, &pb2); //Ya termino el antirrebote?
			//PRINTF("BOTON 3 PRESIONADO\n");
			controlBoton3(&b3, PTB, &PD);
		}


		else
		{

		}


		if(flagPIT0){

			sistemaPrincipal(&b1, PTD, &PD);
			flagPIT0 = 0;

		}

		else if(flagPIT1){
			flagPIT1 = 0;
			reproducirCancion(PTC);
			controlVolumen(&s1, adc16ChannelConfigStruct);
		}

	}


	return 0 ;
}
