/*
 * tiempoBotones.h
 *
 *  Created on: 24 mar. 2020
 *      Author: Diego Moreno
 */

#ifndef TIEMPOBOTONES_H_
#define TIEMPOBOTONES_H_
#define ACTIVE  1
#define INACTIVE 0
#define PIT_CLK_SRC_HZ_HP ((uint64_t)24000000)
#define MIN_TIMER 50
#define MAX_TIMER 999

uint32_t flagPIT2 = 0;


typedef enum
{
	NOT_PRESS,
	PRESS,
	SHORT_PRESS,
	LONG_PRESS,
	MAX_STATES
}DEB_STATES;

DEB_STATES curr_state = NOT_PRESS;
DEB_STATES Next_State = NOT_PRESS;

DEB_STATES DebButton(PinDebounce* p);


DEB_STATES DebButton(PinDebounce* p)
{

	static DEB_STATES ButtonState = NOT_PRESS;
	uint32_t TimerCount = 0;
	uint32_t ElapsedTime = 0;
	unsigned char Debouncedbutton;
	unsigned char button = 0;

	if(button != INACTIVE )
	{
		button = ACTIVE;
	}

	switch(curr_state)
	{
	case NOT_PRESS:

		/*Anti rebote*/
		Debouncedbutton = p->debounced;


		if(Debouncedbutton == INACTIVE)
		{
			/*Timer Stop*/
			PIT_StopTimer(PIT, kPIT_Chnl_2);

			//ShortPress = FALSE;
			//LongPress = FALSE;

			Next_State = NOT_PRESS;
		}
		else /* Button Active*/
		{
			flagPIT2 = 0;
			PIT_StartTimer(PIT, kPIT_Chnl_2);
			Next_State = PRESS;
		}

		ButtonState = NOT_PRESS;

		break;

	case PRESS:

		if(button == ACTIVE)
		{

		}
		else /*button INACTIVE*/
		{
			TimerCount = PIT_GetCurrentTimerCount(PIT, kPIT_Chnl_2);

			ElapsedTime = COUNT_TO_MSEC(TimerCount, PIT_CLK_SRC_HZ_HP);

			if(ElapsedTime > MIN_TIMER && TimerCount <= MAX_TIMER)
			{
				Next_State = SHORT_PRESS;
				ButtonState = SHORT_PRESS;
				//ShortPress = 1;
			}
			else
			{
				Next_State = NOT_PRESS;
				ButtonState = NOT_PRESS;
				PIT_StopTimer(PIT, kPIT_Chnl_2);
			}

		}

		break;

	case SHORT_PRESS:
		break;

	case LONG_PRESS:
		break;

	case MAX_STATES:
	default:

		break;
	}

	curr_state = Next_State;

	return ButtonState;
}



#endif /* TIEMPOBOTONES_H_ */
