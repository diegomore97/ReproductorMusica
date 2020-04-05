/*
 * adctopwm.h
 *
 *  Created on: 23 mar. 2020
 *      Author: Diego Moreno
 */

#ifndef ADCTOPWM_H_
#define ADCTOPWM_H_

#define DEMO_ADC16_BASE ADC0
#define DEMO_ADC16_CHANNEL_GROUP 0U
#define DEMO_ADC16_USER_CHANNEL 0U /*PTE20, ADC0_SE0 */


#define BOARD_TPM_BASEADDR TPM0
#define BOARD_TPM_CHANNEL 2U  //PTE29
#define TPM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_PllFllSelClk)


#define AVANCE_PWM 10
#define MAXIMO_PWM 4095


typedef enum
{
	LEERADC = 1,
	CONVERTIR,
	PWMOUTPUT
}Adcpwm;

typedef struct
{
	uint16_t valorAdc;
	uint8_t dutyCyclePwm;
	Adcpwm curr_state;
	Adcpwm Next_state;
}SensorPwm;

volatile uint8_t updatedDutycycle = 0;
char buffer[25]; //Variable para pruebas


void configPwm(void);
void outputPwm(uint8_t dutyCyclePwm);
void configAdc(adc16_channel_config_t* adc16ChannelConfigStruct);
uint16_t readAdc(adc16_channel_config_t adc16ChannelConfigStruct);
void adctopwm(SensorPwm* s);
void controlVolumen(SensorPwm* s, adc16_channel_config_t canal);
void initSensorPwm(SensorPwm* s);

void initSensorPwm(SensorPwm* s)
{
	s->dutyCyclePwm = 0;
	s->valorAdc = 0;
	s->curr_state = LEERADC;
	s->Next_state = LEERADC;
}

void configPwm(void)
{
	tpm_config_t tpmInfo;
	tpm_pwm_level_select_t pwmLevel = kTPM_HighTrue;
	tpm_chnl_pwm_signal_param_t tpmParam;

	tpmParam.chnlNumber = (tpm_chnl_t)BOARD_TPM_CHANNEL;
	tpmParam.level = pwmLevel;
	tpmParam.dutyCyclePercent = 0;

	CLOCK_SetTpmClock(1U);

	TPM_GetDefaultConfig(&tpmInfo);
	/* Initialize TPM module */
	TPM_Init(BOARD_TPM_BASEADDR, &tpmInfo);

	TPM_SetupPwm(BOARD_TPM_BASEADDR, &tpmParam, 1U, kTPM_CenterAlignedPwm, 24000U, TPM_SOURCE_CLOCK);

	TPM_StartTimer(BOARD_TPM_BASEADDR, kTPM_SystemClock);

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
		//PRINTF("ADC16_DoAutoCalibration() Done.\r\n");
	}
	else
	{
		//PRINTF("ADC16_DoAutoCalibration() Failed.\r\n");
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

void outputPwm(uint8_t dutyCyclePwm)
{
	TPM_UpdatePwmDutycycle(BOARD_TPM_BASEADDR, (tpm_chnl_t)BOARD_TPM_CHANNEL, kTPM_CenterAlignedPwm, dutyCyclePwm);

}

void adctoPWM(SensorPwm* s)
{
	uint8_t duty;

	duty =  (s->valorAdc*100)/MAXIMO_PWM; //REGLA DE 3

	duty -= (duty%AVANCE_PWM); //ELIMINAR UNIDADES PARA SOLO DEJAR LA DECENA

	s->dutyCyclePwm = duty;

}

void controlVolumen(SensorPwm* s, adc16_channel_config_t canal)
{
	unsigned char finish = 0;

	while(!finish){

		switch(s->curr_state)
		{

		case LEERADC:
			s->valorAdc = readAdc(canal);
			s->Next_state = CONVERTIR;
			//sprintf(buffer,"Valor ADC: %d\n", s->valorAdc);
			//PRINTF(buffer);
			break;

		case CONVERTIR:
			adctoPWM(s);
			s->Next_state = PWMOUTPUT;
			break;

		case PWMOUTPUT:
			//sprintf(buffer,"Ciclo de Trabajo: %d\n", s->dutyCyclePwm);
			//PRINTF(buffer);
			outputPwm(s->dutyCyclePwm);
			s->Next_state = LEERADC;
			finish = 1;
			break;

		default:
			break;


		}

		s->curr_state = s->Next_state;

	}

}




#endif /* ADCTOPWM_H_ */
