/*
 * delay.h
 *
 *  Created on: 22 mar. 2020
 *      Author: Diego Moreno
 */

#ifndef DELAY_H_
#define DELAY_H_

static void delay_ms(uint32_t dlyTicks);
volatile uint32_t msTicks = 0;	// counts 1ms timeTicks

//SysTick_Config(SystemCoreClock / 1000);	// Generate interrupt each 1 ms | AGREGAR ESTO AL VOID MAIN


void SysTick_Handler(void) {
	msTicks++;
}


static void delay_ms(uint32_t dlyTicks) {
	uint32_t curTicks;

	curTicks = msTicks;
	while ((msTicks - curTicks) < dlyTicks);

}




#endif /* DELAY_H_ */
