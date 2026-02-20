/**
 * @file led.c
 * @author zhao.wei (hw)
 * @brief 
 * @version 0.1
 * @date 2021-03-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "drv.h"
#include "Ifx_Types.h"
#include "IfxPort.h"
#include "Bsp.h"



void initLEDs(void)
{
    /* Initialize GPIO pins for LEDs */
    IfxPort_setPinMode(LED1, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinMode(LED2, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinMode(LED3, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinMode(LED4, IfxPort_Mode_outputPushPullGeneral);

    /* Turn off all LEDs */
    IfxPort_setPinState(LED1, IfxPort_State_high);
    IfxPort_setPinState(LED2, IfxPort_State_high);
    IfxPort_setPinState(LED3, IfxPort_State_high);
    IfxPort_setPinState(LED4, IfxPort_State_high);

    IfxPort_setPinMode(&MODULE_P33,0,  IfxPort_Mode_outputPushPullGeneral);//蜂鸣器
    IfxPort_setPinLow(&MODULE_P33, 0);
}

