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
//#include "delay.h" //Por si necesito un delay


#define PIT_CLK_SRC_HZ_HP ((uint64_t)24000000)
#define DEMO_ADC16_BASE ADC0
#define DEMO_ADC16_CHANNEL_GROUP 0U
#define DEMO_ADC16_USER_CHANNEL 0U /*PTE20, ADC0_SE0 */
#define TAM_CADENA 40
#define BOARD_TPM_BASEADDR TPM2
#define BOARD_TPM_CHANNEL 1U

#define DEMO_UART UART1
#define DEMO_UART_CLKSRC BUS_CLK
#define DEMO_UART_CLK_FREQ CLOCK_GetFreq(BUS_CLK)


#define TPM_CHANNEL_INTERRUPT_ENABLE kTPM_Chnl1InterruptEnable /* Interrupt to enable and flag to read; depends on the TPM channel used */
#define TPM_CHANNEL_FLAG kTPM_Chnl1Flag /* Interrupt number and interrupt handler for the TPM instance used */
#define TPM_INTERRUPT_NUMBER TPM2_IRQn
#define TPM_LED_HANDLER TPM2_IRQHandler
#define TPM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_PllFllSelClk)/* Get source clock for TPM driver */

uint32_t flagPIT0 = 0;
uint32_t flagPIT1 = 0;
volatile uint8_t updatedDutycycle = 0;
volatile uint8_t basePwm = 5;
uint8_t counter = 0;
uint32_t output = 0;


void PIT_DriverIRQHandler(void);
void configPit(void);
void configPit_2(void);
void configUart(void);
void configPwm(void);
void outputPwm(uint8_t dutyCyclePwm, tpm_pwm_level_select_t pwmLevel);
void configAdc(adc16_channel_config_t* adc16ChannelConfigStruct);
uint16_t readAdc(adc16_channel_config_t adc16ChannelConfigStruct);
uint8_t speedPwm(uint16_t valueAdc, uint8_t* countPwm, bool* increment);
uint8_t speedPwmDecrement(uint16_t valueAdc,uint8_t* countPwm, bool* increment);
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

void configPwm(void)
{
	tpm_config_t tpmInfo;
	tpm_chnl_pwm_signal_param_t tpmParam;
	tpm_pwm_level_select_t pwmLevel = kTPM_LowTrue;

	/* Configure tpm params with frequency 24kHZ */
	tpmParam.chnlNumber = (tpm_chnl_t)BOARD_TPM_CHANNEL;
	tpmParam.level = pwmLevel;
	tpmParam.dutyCyclePercent = updatedDutycycle;

	/* Select the clock source for the TPM counter as kCLOCK_PllFllSelClk */
	CLOCK_SetTpmClock(1U);

	TPM_GetDefaultConfig(&tpmInfo);
	/* Initialize TPM module */
	TPM_Init(BOARD_TPM_BASEADDR, &tpmInfo);

	TPM_SetupPwm(BOARD_TPM_BASEADDR, &tpmParam, 1U, kTPM_CenterAlignedPwm, 24000U, TPM_SOURCE_CLOCK);

	TPM_StartTimer(BOARD_TPM_BASEADDR, kTPM_SystemClock);

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

	PIT_SetTimerPeriod(PIT, kPIT_Chnl_1,MSEC_TO_COUNT(50, PIT_CLK_SRC_HZ_HP));

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

void configAdc(adc16_channel_config_t* adc16ChannelConfigStruct )
{
	adc16_config_t adc16ConfigStruct;

	ADC16_GetDefaultConfig(&adc16ConfigStruct);

#ifdef BOARD_ADC_USE_ALT_VREF
	adc16ConfigStruct.referenceVoltageSource = kADC16_ReferenceVoltageSourceValt;
#endif
	ADC16_Init(DEMO_ADC16_BASE, &adc16ConfigStruct);
	ADC16_EnableHardwareTrigger(DEMO_ADC16_BASE, false); /* Make sure the software trigger is used. */
#if defined(FSL_FEATURE_ADC16_HAS_CALIBRATION) && FSL_FEATURE_ADC16_HAS_CALIBRATION
	if (kStatus_Success == ADC16_DoAutoCalibration(DEMO_ADC16_BASE))
	{
		PRINTF("ADC16_DoAutoCalibration() Done.\r\n");
	}
	else
	{
		PRINTF("ADC16_DoAutoCalibration() Failed.\r\n");
	}
#endif /* FSL_FEATURE_ADC16_HAS_CALIBRATION */

	(*adc16ChannelConfigStruct).channelNumber = DEMO_ADC16_USER_CHANNEL;
	(*adc16ChannelConfigStruct).enableInterruptOnConversionCompleted = false;
#if defined(FSL_FEATURE_ADC16_HAS_DIFF_MODE) && FSL_FEATURE_ADC16_HAS_DIFF_MODE
	(*adc16ChannelConfigStruct).enableDifferentialConversion = false;
#endif /* FSL_FEATURE_ADC16_HAS_DIFF_MODE */

	ADC16_SetChannelConfig(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP, adc16ChannelConfigStruct);
}


uint16_t readAdc(adc16_channel_config_t adc16ChannelConfigStruct)
{
	ADC16_SetChannelConfig(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP, &adc16ChannelConfigStruct);

	while (0U == (kADC16_ChannelConversionDoneFlag & ADC16_GetChannelStatusFlags(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP)));

	return ADC16_GetChannelConversionValue(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP);

}

void outputPwm(uint8_t dutyCyclePwm, tpm_pwm_level_select_t pwmLevel)
{
	updatedDutycycle = dutyCyclePwm;

	/* Disable channel output before updating the dutycycle */
	TPM_UpdateChnlEdgeLevelSelect(BOARD_TPM_BASEADDR, (tpm_chnl_t)BOARD_TPM_CHANNEL, 0U);

	/* Update PWM duty cycle */
	TPM_UpdatePwmDutycycle(BOARD_TPM_BASEADDR, (tpm_chnl_t)BOARD_TPM_CHANNEL, kTPM_CenterAlignedPwm,
			updatedDutycycle);

	/* Start channel output with updated dutycycle */
	TPM_UpdateChnlEdgeLevelSelect(BOARD_TPM_BASEADDR, (tpm_chnl_t)BOARD_TPM_CHANNEL, pwmLevel);

}

uint8_t speedPwm(uint16_t valueAdc,uint8_t* countPwm, bool* increment)
{
	uint8_t basePwmActual;

	if(valueAdc >= 0 && valueAdc < 500)
	{
		basePwmActual = 5;
	}

	else if(valueAdc >= 500 && valueAdc < 1000)
	{
		basePwmActual = 10;
	}

	else if(valueAdc >= 1000 && valueAdc < 1500)
	{
		basePwmActual = 15;
	}

	else if(valueAdc >= 1500 && valueAdc < 2000)
	{
		basePwmActual = 20;
	}

	else if(valueAdc >= 2000 && valueAdc < 2500)
	{
		basePwmActual = 25;
	}

	else if(valueAdc >= 2500 && valueAdc < 3000)
	{
		basePwmActual = 30;
	}

	else if(valueAdc >= 3000 && valueAdc < 3500)
	{
		basePwmActual = 35;
	}

	else if(valueAdc >= 3500 && valueAdc < 4000)
	{
		basePwmActual = 40;
	}

	else
	{
		basePwmActual = 50;
	}

	*countPwm += basePwmActual;

	if(*countPwm >= 100)
	{
		*countPwm = 100;
		*increment = false;
	}
	else
	{

	}

	return *countPwm;

}

uint8_t speedPwmDecrement(uint16_t valueAdc,uint8_t* countPwm, bool* increment)
{
	uint8_t basePwmActual;
	int aux = *countPwm;

	if(valueAdc >= 0 && valueAdc < 500)
	{
		basePwmActual = 5;
	}

	else if(valueAdc >= 500 && valueAdc < 1000)
	{
		basePwmActual = 10;
	}

	else if(valueAdc >= 1000 && valueAdc < 1500)
	{
		basePwmActual = 15;
	}

	else if(valueAdc >= 1500 && valueAdc < 2000)
	{
		basePwmActual = 20;
	}

	else if(valueAdc >= 2000 && valueAdc < 2500)
	{
		basePwmActual = 25;
	}

	else if(valueAdc >= 2500 && valueAdc < 3000)
	{
		basePwmActual = 30;
	}

	else if(valueAdc >= 3000 && valueAdc < 3500)
	{
		basePwmActual = 35;
	}

	else if(valueAdc >= 3500 && valueAdc < 4000)
	{
		basePwmActual = 40;
	}

	else
	{
		basePwmActual = 50;
	}

	aux -= basePwmActual;

	if((aux) <= 0)
	{
		aux = 0;
		*increment = true;
	}
	else
	{

	}

	*countPwm = aux;

	return *countPwm;

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

	case 0:
		base->PDDR = 0; //EL TRISTE - JOSE JOSE
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

	uint16_t valueAdc = 0;
	uint8_t dutyCyclePwm = 0;
	uint8_t countPwm = 0;
	bool increment = true;
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
	initPinDebounce(&pb0 , 1, 3); //Inicializar configuracion | Parametros 1 y 3 relacionados con la velocidad de pulsado

	//BOTON NEXT | FWD
	initPinDebounce(&pb1 , 1, 3);

	//BOTON PREW | BWD
	initPinDebounce(&pb2 , 1, 3);

	initPortRotabit(&PD, 3); //Numero de leds | Asegurese que los pines de las puerto esten configurados como salidas en LOGICA 1

	BotonControl b1, b2, b3;

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
		}

	}


	return 0 ;
}
