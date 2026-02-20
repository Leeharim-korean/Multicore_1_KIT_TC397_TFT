/**
 * @file drv.h
 * @author zhao.wei (hw)
 * @brief 
 * @version 0.1
 * @date 2021-03-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef DRV_H_
#define DRV_H_

#include "Bsp.h"
#include "Cpu/Std/Ifx_Types.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxPort.h"
#include "IfxQspi_SpiMaster.h"
#include "IfxQspi_SpiSlave.h"
#include "IfxAsclin_Asc.h"
#include "IfxGeth_Eth.h"
#include "IfxGeth.h"
#include "Ifx_Shell.h"
#include "Ifx_Console.h"
#include "Ifx_Assert.h"
#include "IfxStdIf_DPipe.h"
#include "_Utilities/Ifx_Assert.h"
#include "IfxCpu_Intrinsics.h"
#include "IfxPort.h"
#include "IfxQspi_SpiMaster.h"
#include "IfxStm.h"
#include "Assert.h"
#include "Can\Can\IfxCan_Can.h"
/*************驱动的中断优先级*********************************/

// 1 and 2 used for freertos
#define IFX_INTPRIO_STM0_SR0 4
#define ISR_PRIORITY_RxEth 5
#define ISR_PRIORITY_TxEth 6
#define ISR_PRIORITY_ASCLIN_TX 7 /* Priority for interrupt ISR Transmit  */
#define ISR_PRIORITY_ASCLIN_RX 8 /* Priority for interrupt ISR Receive   */
#define ISR_PRIORITY_ASCLIN_ER 9 /* Priority for interrupt ISR Errors    */
#define IFX_INTPRIO_CAN0_RX 10
#define IFX_INTPRIO_CAN1_RX 11
#define IFX_INTPRIO_CAN2_RX 12
/***********************************************************/

struct drv_asc_lin_uart
{
#define ASC_TX_BUFFER_SIZE 64 /* Define the TX buffer size in byte    */
#define ASC_RX_BUFFER_SIZE 64 /* Define the RX buffer size in byte    */
    uint8 uart_tx_buffer[ASC_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
    uint8 uart_rx_buffer[ASC_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];

    IfxStdIf_DPipe asc_sandard_interface; /* Standard interface object            */
    IfxAsclin_Asc asclin;                 /* ASCLIN module object                 */
};

struct drv_eth
{
    IfxGeth_Eth eth;
};

struct drv_can
{
#define CAN_NUM 12
    IfxCan_Can_Node can_node;              /* CAN source node handle data structure                  */
    IfxCan_Can_NodeConfig can_node_config; /* CAN node configuration structure                       */
    IfxCan_Message rx_msg;
    IfxCan_Message tx_msg;
    uint32 rxdata[2];
    uint32 txdata[2];
};


/* LEDs */
#define LED1 &MODULE_P13, 0 /* LED D107                             */
#define LED2 &MODULE_P13, 1 /* LED D108                             */
#define LED3 &MODULE_P13, 2 /* LED D109                             */
#define LED4 &MODULE_P13, 3 /* LED D110                             */

IFX_EXTERN struct drv_asc_lin_uart g_drv_asc_lin_uart;
IFX_EXTERN struct drv_eth g_drv_eth;
IFX_EXTERN struct drv_can g_drv_can[CAN_NUM];

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/
void initShellInterface(void);
void Ifx_print(pchar format, ...);

void initLEDs(void);

void init_eth_module(uint8 *mac_addr);
int rtl8211f_init_config(void);
int rtl8211f_parse_status(void);
boolean rtl8211f_link_state(void);
boolean rtl8211f_link(void);

void instruct_delay(int times);
void stm_init(void);

void can_send(int index);
void can_init(int index);
//

#endif /* ASCLIN_SHELL_UART_H_ */
