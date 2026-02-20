/**
 * @file pub.h
 * @author zhao.wei (hw)
 * @brief 
 * @version 0.1
 * @date 2021-03-05
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef PUB_H_
#define PUB_H_

#include "Bsp.h"
#include "Cpu/Std/Ifx_Types.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"
#include "string.h"

#include "Ifx_Types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "drv.h"
#include "semphr.h"

#include <netif.h>
#include <tcpip.h>
#include <etharp.h>
#include <ethernet.h>
#include <ethernetif.h>
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "sockets.h"
#include "udp.h"
#include "ip4_addr.h"

//#define CAN_TEST_CPU0//can测试在cpu0进行，可以用debug打印
#define CAN_TEST_CPU1
#define ETH_TEST_CPU0

extern SemaphoreHandle_t g_eth_swamphore;
extern SemaphoreHandle_t g_can0_swamphore;
extern SemaphoreHandle_t g_can2_swamphore;
extern int can0_recv_times, can2_recv_times;
extern BaseType_t ret0, ret1, ret2;

void test_can0_send_cpu0(void *pvParameters);
void test_can0_recv_cpu0(void *pvParameters);
void test_can2_send_cpu0(void *pvParameters);
void test_can2_recv_cpu0(void *pvParameters);

void test_can0_send_cpu1(void *pvParameters);
void test_can0_recv_cpu1(void *pvParameters);
void test_can2_send_cpu1(void *pvParameters);
void test_can2_recv_cpu1(void *pvParameters);
#endif
