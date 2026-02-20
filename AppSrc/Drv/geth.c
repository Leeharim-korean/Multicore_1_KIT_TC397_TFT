/**
 * @file geth.c
 * @author zhao.wei (hw)
 * @brief 
 * @version 0.1
 * @date 2021-03-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "drv.h"
#include "Ifx_Types.h"
#include "IfxPort.h"
#include "Bsp.h"

struct drv_eth g_drv_eth;
uint8 myMacAddress[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
boolean phyLink = 0;

#define IFXGETH_MAX_TX_BUFFER_SIZE 1536 // bytes
#define IFXGETH_MAX_RX_BUFFER_SIZE 1536 // bytes

uint8 channel0TxBuffer[IFXGETH_MAX_TX_DESCRIPTORS][IFXGETH_MAX_TX_BUFFER_SIZE];
uint8 channel0RxBuffer[IFXGETH_MAX_RX_DESCRIPTORS][IFXGETH_MAX_RX_BUFFER_SIZE];

const IfxGeth_Eth_RgmiiPins rgmii_pins = {
    &IfxGeth_TXCLK_P11_4_OUT,
    &IfxGeth_TXD0_P11_3_OUT,
    &IfxGeth_TXD1_P11_2_OUT,
    &IfxGeth_TXD2_P11_1_OUT,
    &IfxGeth_TXD3_P11_0_OUT,
    &IfxGeth_TXCTL_P11_6_OUT,
    &IfxGeth_RXCLKA_P11_12_IN,
    &IfxGeth_RXD0A_P11_10_IN,
    &IfxGeth_RXD1A_P11_9_IN,
    &IfxGeth_RXD2A_P11_8_IN,
    &IfxGeth_RXD3A_P11_7_IN,
    &IfxGeth_RXCTLA_P11_11_IN,
    &IfxGeth_MDC_P12_0_OUT,
    &IfxGeth_MDIO_P12_1_INOUT,
    &IfxGeth_GREFCLK_P11_5_IN};

void init_eth_module(uint8 *mac_addr)
{
    IfxGeth_Eth_Config geth_config;
    IfxGeth_Index gethInst;
    Ifx_print("init_eth_module 1\r\n");
    IfxGeth_Eth_initModuleConfig(&geth_config, &MODULE_GETH);
    gethInst = 0;

    geth_config.phyInterfaceMode = IfxGeth_PhyInterfaceMode_rgmii;
    geth_config.pins.rgmiiPins = &rgmii_pins;

    /* setup the config struct */
    geth_config.mac.lineSpeed = IfxGeth_LineSpeed_1000Mbps;
    geth_config.mac.loopbackMode = IfxGeth_LoopbackMode_disable;
    geth_config.mac.duplexMode = IfxGeth_DuplexMode_fullDuplex;
    memcpy(geth_config.mac.macAddress, mac_addr, 6);

    geth_config.mtl.numOfTxQueues = 1;
    geth_config.mtl.txSchedulingAlgorithm = IfxGeth_TxSchedulingAlgorithm_wrr; //
    geth_config.mtl.numOfRxQueues = 1;
    geth_config.mtl.rxArbitrationAlgorithm = IfxGeth_RxArbitrationAlgorithm_sp; //
    geth_config.mtl.txQueue[0].txQueueUnderflowInterruptEnabled = FALSE;        //
    geth_config.mtl.txQueue[0].txQueueSize = IfxGeth_QueueSize_1536Bytes;
    geth_config.mtl.rxQueue[0].rxQueueSize = IfxGeth_QueueSize_1536Bytes;
    geth_config.mtl.rxQueue[0].rxDmaChannelMap = IfxGeth_RxDmaChannel_0;

    geth_config.dma.numOfTxChannels = 1;
    geth_config.dma.numOfRxChannels = 1;
    geth_config.dma.txChannel[0].channelId = IfxGeth_TxDmaChannel_0;
    geth_config.dma.txChannel[0].txDescrList = &IfxGeth_Eth_txDescrList[gethInst][0];                                                 //Initialise tx desc list for the Geth instance
    geth_config.dma.txChannel[0].txBuffer1StartAddress = (uint32 *)IFXCPU_GLB_ADDR_DSPR(IfxCpu_getCoreId(), &channel0TxBuffer[0][0]); // user buffer
    geth_config.dma.txChannel[0].txBuffer1Size = IFXGETH_MAX_TX_BUFFER_SIZE;                                                          // used to calculate the next descriptor  buffer offset
    geth_config.dma.rxChannel[0].channelId = IfxGeth_RxDmaChannel_0;
    geth_config.dma.rxChannel[0].rxDescrList = &IfxGeth_Eth_rxDescrList[gethInst][0];                                                 //Initialise rx desc list for the Geth instance
    geth_config.dma.rxChannel[0].rxBuffer1StartAddress = (uint32 *)IFXCPU_GLB_ADDR_DSPR(IfxCpu_getCoreId(), &channel0RxBuffer[0][0]); // user buffer
    geth_config.dma.rxChannel[0].rxBuffer1Size = IFXGETH_MAX_RX_BUFFER_SIZE;                                                          // user defined variable
    geth_config.dma.txInterrupt[0].channelId = IfxGeth_DmaChannel_0;
    geth_config.dma.txInterrupt[0].priority = ISR_PRIORITY_TxEth;
    geth_config.dma.txInterrupt[0].provider = IfxSrc_Tos_cpu0;
    geth_config.dma.rxInterrupt[0].channelId = IfxGeth_DmaChannel_0;
    geth_config.dma.rxInterrupt[0].priority = ISR_PRIORITY_RxEth; //中断响应函数在lwip_freertos_demo.c
    geth_config.dma.rxInterrupt[0].provider = IfxSrc_Tos_cpu0;

    IfxGeth_Eth_initModule(&g_drv_eth.eth, &geth_config);

    Ifx_print("init_eth_module 2\r\n");
    // iniialise RTL8211FI
    rtl8211f_init_config();
    //waitTime(TimeConst_1s * 5);
    //rtl8211f_parse_status();
    //waitTime(TimeConst_1s * 3);
    instruct_delay(2);
    if (rtl8211f_link() == 1) //LINK Status
    {
        Ifx_print("rtl8211f_link is ok !\r\n");
    }
    else
    {
        Ifx_print("rtl8211f_link is error !\r\n");
    }
    IfxGeth_Eth_startTransmitters(&g_drv_eth.eth, 1);
    IfxGeth_Eth_startReceivers(&g_drv_eth.eth, 1);
}
