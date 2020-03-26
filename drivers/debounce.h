/*
 * debounce.h
 *
 *  Created on: 20 mar. 2020
 *      Author: Diego Moreno
 */

#ifndef DEBOUNCE_H_
#define DEBOUNCE_H_

typedef enum
{

	DISABLE = 1,
	COUNT_ENABLE,
	ENABLE,
	COUNT_DIS,

}DEBOUNCE_STATES;


typedef struct
{
	uint8_t counter;
	DEBOUNCE_STATES curr_state;
	DEBOUNCE_STATES Next_state;
	uint8_t minCountEnable; //RELACIONADO CON LA VELOCIDAD DE PULSACION
	uint8_t minCountDis;
	unsigned char debounced;

}PinDebounce;

void initPinDebounce(PinDebounce* p, uint8_t minCountEnable, uint8_t minCountDis); //Prototype function
void antiBounceButtonPullUp(GPIO_Type *base, uint32_t pinLeer, PinDebounce* p); //Prototype function
void antiBounceButtonPullDown(GPIO_Type *base, uint32_t pinLeer, PinDebounce* p); //Prototype function

void initPinDebounce(PinDebounce* p, uint8_t minCountEnable, uint8_t minCountDis)
{
	p->counter = 0;
	p->Next_state = DISABLE;
	p->curr_state = DISABLE;
	p->minCountEnable = minCountEnable; //RELACIONADO CON LA VELOCIDAD DE PULSACION
	p->minCountDis = minCountDis;
	p->debounced = 0;
}

void antiBounceButtonPullDown(GPIO_Type *base, uint32_t pinLeer, PinDebounce* p) //make sure you have the resistor in pulldown
{

	switch(p->curr_state)
	{

	case DISABLE:

		//PRINTF("DEBOUNCE: STATE DISABLE\n");

		if(GPIO_ReadPinInput(base , pinLeer) )
		{
			p->Next_state = COUNT_ENABLE;
			p->counter = 0;
			p->debounced = 0;
		}

		else
		{
			p->Next_state = DISABLE;
		}

		break;

	case COUNT_ENABLE:

		//PRINTF("DEBOUNCE: STATE COUNT_ENABLE\n");

		if(GPIO_ReadPinInput(base , pinLeer) )
		{
			p->counter+=1;

			if(p->counter >= p->minCountEnable)
			{
				p->Next_state = ENABLE;
			}

			else
			{
				p->Next_state = COUNT_ENABLE;
			}

		}

		else
		{
			p->Next_state = DISABLE;
			p->counter = 0;
		}

		break;

	case ENABLE:

		//PRINTF("DEBOUNCE: STATE ENABLE\n");

		if(!(GPIO_ReadPinInput(base , pinLeer)) ) //INPUT = LOW
		{
			p->counter = 0;
			p->Next_state = COUNT_DIS;

		}

		break;

	case COUNT_DIS:

		//PRINTF("DEBOUNCE: STATE COUNT_DIS\n");

		if(!(GPIO_ReadPinInput(base , pinLeer)) ) //INPUT = LOW
		{

			p->counter+=1;

			if(p->counter >= p->minCountDis)
			{
				p->Next_state = DISABLE;
				p->debounced = 1;
			}

			else
			{
				p->Next_state = COUNT_DIS;
			}


		}

		else
		{
			p->counter = 0;
			p->Next_state = ENABLE;
		}

		break;


	default:
		break;

	}

	p->curr_state = p->Next_state;


}

void antiBounceButtonPullUp(GPIO_Type *base, uint32_t pinLeer, PinDebounce* p) //make sure you have the resistor in pullup either by sw or hw

{

	switch(p->curr_state)
	{

	case DISABLE:

		//PRINTF("STATE DISABLE\n");

		if(!(GPIO_ReadPinInput(base , pinLeer) ) )
		{
			p->Next_state = COUNT_ENABLE;
			p->counter = 0;
		}

		else
		{
			p->Next_state = DISABLE;
			p->debounced = 0;
		}

		break;

	case COUNT_ENABLE:

		//PRINTF("STATE COUNT_ENABLE\n");

		if(!(GPIO_ReadPinInput(base , pinLeer) ) )
		{
			p->counter+=1;

			if(p->counter >= p->minCountEnable)
			{
				p->Next_state = ENABLE;
			}

			else
			{
				p->Next_state = COUNT_ENABLE;
			}

		}

		else
		{
			p->Next_state = DISABLE;
			p->counter = 0;
		}

		break;

	case ENABLE:

		//PRINTF("STATE ENABLE\n");

		if(GPIO_ReadPinInput(base , pinLeer))  //INPUT = ON
		{
			p->counter = 0;
			p->Next_state = COUNT_DIS;

		}

		break;

	case COUNT_DIS:

		//PRINTF("STATE COUNT_DIS\n");

		if(GPIO_ReadPinInput(base , pinLeer))  //INPUT = ON
		{

			p->counter+=1;

			if(p->counter >= p->minCountDis)
			{
				p->Next_state = DISABLE;
				p->debounced = 1;
			}

			else
			{
				p->Next_state = COUNT_DIS;
			}


		}

		else
		{
			p->counter = 0;
			p->Next_state = ENABLE;
		}

		break;


	default:
		break;

	}


	p->curr_state = p->Next_state;


}



#endif /* DEBOUNCE_H_ */
