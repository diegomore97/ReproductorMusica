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
	uint32_t output;
	DEBOUNCE_STATES curr_state;
	DEBOUNCE_STATES Next_state;
	uint8_t minCountEnable; //RELACIONADO CON LA VELOCIDAD DE PULSACION
	uint8_t minCountDis;

}PinDebounce;

void initPinDebounce(PinDebounce* p, uint8_t minCountEnable, uint8_t minCountDis); //Prototype function
void antiBounceButtonPullUp(GPIO_Type *base, uint32_t pinLeer, PinDebounce* p); //Prototype function
void antiBounceButtonPullDown(GPIO_Type *base, uint32_t pinLeer, PinDebounce* p); //Prototype function

void initPinDebounce(PinDebounce* p, uint8_t minCountEnable, uint8_t minCountDis)
{
	p->counter = 0;
	p->output = DISABLE;
	p->Next_state = DISABLE;
	p->curr_state = DISABLE;
	p->minCountEnable = minCountEnable; //RELACIONADO CON LA VELOCIDAD DE PULSACION
	p->minCountDis = minCountDis;
}

void antiBounceButtonPullDown(GPIO_Type *base, uint32_t pinLeer, PinDebounce* p) //make sure you have the resistor in pulldown
{

	unsigned char finish = 0;

	while(!finish)
	{

		switch(p->curr_state)
		{

		case DISABLE:

			//PRINTF("DEBOUNCE: STATE DISABLE\n");

			if(GPIO_ReadPinInput(base , pinLeer) )
			{
				p->Next_state = COUNT_ENABLE;
				p->counter = 0;
				p->output=DISABLE;
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
					p->output = ENABLE;
				}

				else
				{
					p->Next_state = COUNT_ENABLE;
					p->output = DISABLE;
				}

			}

			else
			{
				p->Next_state = DISABLE;
				p->counter = 0;
				p->output = DISABLE;
			}

			break;

		case ENABLE:

			//PRINTF("DEBOUNCE: STATE ENABLE\n");

			if(!(GPIO_ReadPinInput(base , pinLeer)) ) //INPUT = LOW
			{
				p->counter = 0;
				p->output = ENABLE;
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
					p->output = DISABLE;
					finish = 1;
				}

				else
				{
					p->output = ENABLE;
					p->Next_state = COUNT_DIS;
				}


			}

			else
			{
				p->counter = 0;
				p->Next_state = ENABLE;
				p->output = ENABLE;
			}

			break;


		default:
			break;

		}

		p->curr_state = p->Next_state;

	}

}

void antiBounceButtonPullUp(GPIO_Type *base, uint32_t pinLeer, PinDebounce* p) //make sure you have the resistor in pullup either by sw or hw

{

	unsigned char finish = 0;

	while(!finish)
	{

		switch(p->curr_state)
		{

		case DISABLE:

			//PRINTF("STATE DISABLE\n");

			if(!(GPIO_ReadPinInput(base , pinLeer) ) )
			{
				p->Next_state = COUNT_ENABLE;
				p->counter = 0;
				p->output=DISABLE;
			}

			else
			{
				p->Next_state = DISABLE;
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
					p->output = ENABLE;
				}

				else
				{
					p->Next_state = COUNT_ENABLE;
					p->output = DISABLE;
				}

			}

			else
			{
				p->Next_state = DISABLE;
				p->counter = 0;
				p->output = DISABLE;
			}

			break;

		case ENABLE:

			//PRINTF("STATE ENABLE\n");

			if(GPIO_ReadPinInput(base , pinLeer))  //INPUT = ON
			{
				p->counter = 0;
				p->output = ENABLE;
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
					p->output = DISABLE;
					finish = 1;
				}

				else
				{
					p->output = ENABLE;
					p->Next_state = COUNT_DIS;
				}


			}

			else
			{
				p->counter = 0;
				p->Next_state = ENABLE;
				p->output = ENABLE;
			}

			break;


		default:
			break;

		}


		p->curr_state = p->Next_state;

	}

}



#endif /* DEBOUNCE_H_ */
