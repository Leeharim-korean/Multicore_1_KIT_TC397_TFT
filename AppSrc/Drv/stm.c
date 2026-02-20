/**
 * @file stm.c
 * @author zhao.wei (hw)
 * @brief 
 * @version 0.1
 * @date 2021-03-12
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "drv.h"
#include "Ifx_Types.h"
#include "IfxPort.h"
#include "Bsp.h"

static uint32 stm_led_ind = 0;

IfxStm_CompareConfig stmCompareConfig;
void stm_init(void)
{
    /* Initialize STM for the triggers*/

    /* Call the constructor of configuration */
    IfxStm_initCompareConfig(&stmCompareConfig);

    /* Calculate the compare value of STM0 */
    IfxStm_enableOcdsSuspend(&MODULE_STM5);
    /* Configure interrupt service requests for STM trigger outputs */
    // configure to generate interrupt every 1000 us
    sint32 ticks = IfxStm_getTicksFromMicroseconds(&MODULE_STM5, 1000);
    /* Modify only the number of ticks and enable the trigger output */
    stmCompareConfig.ticks = ticks;                                        /*Interrupt after stm0CompareValue ticks from now */
    stmCompareConfig.comparatorInterrupt = IfxStm_ComparatorInterrupt_ir0; //User must select the interrupt output
    stmCompareConfig.triggerPriority = IFX_INTPRIO_STM0_SR0;
    stmCompareConfig.typeOfService = IfxSrc_Tos_cpu0;
    /* Now Compare functionality is initialized */
    IfxStm_initCompare(&MODULE_STM5, &stmCompareConfig);
    IfxCpu_enableInterrupts();
}

IFX_INTERRUPT(stm0Sr0ISR, 0, IFX_INTPRIO_STM0_SR0);

void stm0Sr0ISR(void)
{
    IfxStm_clearCompareFlag(&MODULE_STM0, stmCompareConfig.comparator);
    IfxStm_increaseCompare(&MODULE_STM0, stmCompareConfig.comparator, stmCompareConfig.ticks);
#if 0
	if(phyLink)
	{
		if(stm_led_ind > 500)
		{
			stm_led_ind = 0;
			IfxPort_togglePin(&MODULE_P13, 0);
			IfxPort_togglePin(&MODULE_P13, 3);
			if(Start_cmd==1)
			{
			 IfxPort_togglePin(&MODULE_P33, 0);//蜂鸣器
			}
			else  IfxPort_setPinLow(&MODULE_P33, 0);
		}
		stm_led_ind++;
	}
	 __enable();
#endif
}
