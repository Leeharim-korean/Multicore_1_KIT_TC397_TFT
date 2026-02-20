/**
 * @file can.c
 * @author zhao.wei (hw)
 * @brief 
 * @version 0.1
 * @date 2021-03-17
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "drv.h"
#include "pub.h"
#include "Ifx_Types.h"
#include "IfxPort.h"
#include "Bsp.h"

#define CAN_MESSAGE_ID 0x777

#define MODULE_CAN0_RAM 0xF0200000
#define MODULE_CAN1_RAM 0xF0210000
#define MODULE_CAN2_RAM 0xF0220000

#define NODE0_RAM_OFFSET 0x0
#define NODE1_RAM_OFFSET 0x1000

#define NODE_RAM_OFFSET 0x1000
struct drv_can g_drv_can[CAN_NUM];
int can0_recv_times = 0, can2_recv_times = 0;
SemaphoreHandle_t g_can0_swamphore = NULL;
SemaphoreHandle_t g_can2_swamphore = NULL;

IFX_CONST IfxCan_Can_Pins can_pins[CAN_NUM] = {
    {&IfxCan_TXD00_P20_8_OUT, IfxPort_OutputMode_pushPull, // Tx pin
     &IfxCan_RXD00B_P20_7_IN, IfxPort_InputMode_pullUp,    // Rx pin
     IfxPort_PadDriver_cmosAutomotiveSpeed4},
    {NULL, IfxPort_OutputMode_pushPull, // Tx pin
     NULL, IfxPort_InputMode_pullUp,    // Rx pin
     IfxPort_PadDriver_cmosAutomotiveSpeed4},
    {&IfxCan_TXD02_P10_3_OUT, IfxPort_OutputMode_pushPull, // Tx pin
     &IfxCan_RXD02E_P10_2_IN, IfxPort_InputMode_pullUp,    // Rx pin
     IfxPort_PadDriver_cmosAutomotiveSpeed4},
    {NULL, IfxPort_OutputMode_pushPull, // Tx pin
     NULL, IfxPort_InputMode_pullUp,    // Rx pin
     IfxPort_PadDriver_cmosAutomotiveSpeed4},
    {NULL, IfxPort_OutputMode_pushPull, // Tx pin
     NULL, IfxPort_InputMode_pullUp,    // Rx pin
     IfxPort_PadDriver_cmosAutomotiveSpeed4},
    {NULL, IfxPort_OutputMode_pushPull, // Tx pin
     NULL, IfxPort_InputMode_pullUp,    // Rx pin
     IfxPort_PadDriver_cmosAutomotiveSpeed4},
    {NULL, IfxPort_OutputMode_pushPull, // Tx pin
     NULL, IfxPort_InputMode_pullUp,    // Rx pin
     IfxPort_PadDriver_cmosAutomotiveSpeed4},
    {NULL, IfxPort_OutputMode_pushPull, // Tx pin
     NULL, IfxPort_InputMode_pullUp,    // Rx pin
     IfxPort_PadDriver_cmosAutomotiveSpeed4},
    {NULL, IfxPort_OutputMode_pushPull, // Tx pin
     NULL, IfxPort_InputMode_pullUp,    // Rx pin
     IfxPort_PadDriver_cmosAutomotiveSpeed4},
    {NULL, IfxPort_OutputMode_pushPull, // Tx pin
     NULL, IfxPort_InputMode_pullUp,    // Rx pin
     IfxPort_PadDriver_cmosAutomotiveSpeed4},
    {NULL, IfxPort_OutputMode_pushPull, // Tx pin
     NULL, IfxPort_InputMode_pullUp,    // Rx pin
     IfxPort_PadDriver_cmosAutomotiveSpeed4},
    {NULL, IfxPort_OutputMode_pushPull, // Tx pin
     NULL, IfxPort_InputMode_pullUp,    // Rx pin
     IfxPort_PadDriver_cmosAutomotiveSpeed4},
};

void can_init(int index)
{
    // create module configuration
    IfxCan_Can_Config canConfig;
    IfxCan_Can can;
    IfxCan_NodeId can_node_num = 0;
    //IfxCan_Can_initModuleConfig(&canConfig, &MODULE_CAN0);
    if (index >= 0 && index < 4)
    {
        IfxCan_Can_initModuleConfig(&canConfig, &MODULE_CAN0);
    }
    if (index >= 4 && index < 8)
    {
        IfxCan_Can_initModuleConfig(&canConfig, &MODULE_CAN1);
    }
    if (index >= 8 && index < 12)
    {
        IfxCan_Can_initModuleConfig(&canConfig, &MODULE_CAN2);
    }
    // initialize module
    // IfxCan_Can can; defined globally
    IfxCan_Can_initModule(&can, &canConfig);
    can_node_num = index % 4;

    // create node configuration
    {

        IfxCan_Can_initNodeConfig(&g_drv_can[index].can_node_config, &can);
        // initialize Node 0
        g_drv_can[index].can_node_config.nodeId = can_node_num;
        g_drv_can[index].can_node_config.clockSource = IfxCan_ClockSource_both;
        g_drv_can[index].can_node_config.frame.type = IfxCan_FrameType_transmitAndReceive;
        g_drv_can[index].can_node_config.frame.mode = IfxCan_FrameMode_standard;
        g_drv_can[index].can_node_config.txConfig.dedicatedTxBuffersNumber = 255;
        g_drv_can[index].can_node_config.txConfig.txMode = IfxCan_TxMode_dedicatedBuffers;
        g_drv_can[index].can_node_config.txConfig.txBufferDataFieldSize = IfxCan_DataFieldSize_8;

        g_drv_can[index].can_node_config.rxConfig.rxMode = IfxCan_RxMode_fifo0;
        g_drv_can[index].can_node_config.rxConfig.rxFifo0Size = 255;
        //g_drv_can[index].can_node_config.filterConfig.extendedListSize = 255;
        //g_drv_can[index].can_node_config.filterConfig.messageIdLength = IfxCan_MessageIdLength_extended;

        g_drv_can[index].can_node_config.baudRate.baudrate = 500000; //// 250 kBaud

        g_drv_can[index].can_node_config.interruptConfig.rxFifo0NewMessageEnabled = TRUE;
        g_drv_can[index].can_node_config.interruptConfig.rxf0n.priority = IFX_INTPRIO_CAN0_RX + index;
#ifdef CAN_TEST_CPU0
        g_drv_can[index].can_node_config.interruptConfig.rxf0n.typeOfService = IfxSrc_Tos_cpu0;
#endif
#ifdef CAN_TEST_CPU1
        g_drv_can[index].can_node_config.interruptConfig.rxf0n.typeOfService = IfxSrc_Tos_cpu1;
#endif
        g_drv_can[index].can_node_config.interruptConfig.rxf0n.interruptLine = IfxCan_InterruptLine_0 + index;

        g_drv_can[index].can_node_config.pins = &can_pins[index];
        g_drv_can[index].can_node_config.busLoopbackEnabled = FALSE;
        g_drv_can[index].can_node_config.calculateBitTimingValues = TRUE;
        // initialize Node 0
        IfxCan_Can_initNode(&g_drv_can[index].can_node, &g_drv_can[index].can_node_config);
    }
// set filter for RxBuffer 0
#if 0
    {
        // Initialize the filter structure
        IfxCan_Filter filter;
        filter.number = 0;
        filter.elementConfiguration = IfxCan_FilterElementConfiguration_storeInRxBuffer;
        filter.type = IfxCan_FilterType_none;
        filter.id1 = 0x111; //
        filter.rxBufferOffset = IfxCan_RxBufferId_0;
        IfxCan_Can_setExtendedFilter(&g_drv_can[index].can_node, &filter);
    }
#endif
}

#ifdef CAN_TEST_CPU0
IFX_INTERRUPT(Can0RxISR, 0, IFX_INTPRIO_CAN0_RX);
#endif
#ifdef CAN_TEST_CPU1
IFX_INTERRUPT(Can0RxISR, 1, IFX_INTPRIO_CAN0_RX);
#endif


void Can0RxISR(void)
{
    BaseType_t xHigherPriorityTaskWoken;
    IfxCan_Message rx_msg;
    Ifx_CAN_N *nodeSfr = IfxCan_getNodePointer(&MODULE_CAN0, (IfxCan_NodeId)0);
    can0_recv_times++;
    xHigherPriorityTaskWoken = pdFALSE;

    IfxCan_Can_initMessage(&g_drv_can[0].rx_msg);

    g_drv_can[0].rx_msg.bufferNumber = 0;
    g_drv_can[0].rx_msg.readFromRxFifo0 = TRUE;
    // read message
    IfxCan_Can_readMessage(&g_drv_can[0].can_node, &g_drv_can[0].rx_msg, g_drv_can[0].rxdata);
    if (g_can0_swamphore != NULL) //给出信号量
    {
        xSemaphoreGiveFromISR(g_can0_swamphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    IfxCan_Node_clearInterruptFlag(nodeSfr, IfxCan_Interrupt_rxFifo0NewMessage);
}

#ifdef CAN_TEST_CPU0
IFX_INTERRUPT(Can2RxISR, 0, IFX_INTPRIO_CAN2_RX);
#endif
#ifdef CAN_TEST_CPU1
IFX_INTERRUPT(Can2RxISR, 1, IFX_INTPRIO_CAN2_RX);
#endif


void Can2RxISR(void)
{
    BaseType_t xHigherPriorityTaskWoken;
    IfxCan_Message rx_msg;
    Ifx_CAN_N *nodeSfr = IfxCan_getNodePointer(&MODULE_CAN0, (IfxCan_NodeId)2);
    can2_recv_times++;
    IfxCan_Can_initMessage(&g_drv_can[2].rx_msg);

    g_drv_can[2].rx_msg.bufferNumber = 0;
    g_drv_can[2].rx_msg.readFromRxFifo0 = TRUE;
    // read message
    IfxCan_Can_readMessage(&g_drv_can[2].can_node, &g_drv_can[2].rx_msg, g_drv_can[2].rxdata);
    if (g_can2_swamphore != NULL) //给出信号量
    {
        xSemaphoreGiveFromISR(g_can2_swamphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    IfxCan_Node_clearInterruptFlag(nodeSfr, IfxCan_Interrupt_rxFifo0NewMessage);
}

void can_send(int index)
{
    IfxCan_Can_initMessage(&g_drv_can[index].rx_msg);
    // change the relevent members of message structure
    g_drv_can[index].tx_msg.bufferNumber = 0;
    g_drv_can[index].tx_msg.dataLengthCode = IfxCan_DataLengthCode_8;
    g_drv_can[index].tx_msg.frameMode = IfxCan_FrameMode_standard;
    g_drv_can[index].tx_msg.messageIdLength = IfxCan_MessageIdLength_standard;
    g_drv_can[index].tx_msg.messageId = CAN_MESSAGE_ID;
    g_drv_can[index].txdata[0] = 0x78563412;
    g_drv_can[index].txdata[1] = 0xf0debc9a;
    // Transmit Data
    while (IfxCan_Can_sendMessage(&g_drv_can[index].can_node, &g_drv_can[index].tx_msg, g_drv_can[index].txdata) == IfxCan_Status_notSentBusy)
        ;
}
