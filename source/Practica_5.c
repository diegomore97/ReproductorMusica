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
#include "sistemaPrincipal.h"

void PIT_DriverIRQHandler(void);

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

int main(void) {

	adc16_channel_config_t adc16ChannelConfigStruct;

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();

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
		visualizarAvanceCancion(&PR);
		visualizarCancionActual();
		controlVolumen(&s1, adc16ChannelConfigStruct);

	}


	return 0 ;
}
