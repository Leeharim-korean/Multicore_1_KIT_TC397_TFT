#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "pub.h"

void test_can0_send_cpu0(void *pvParameters)
{
    while (1)
    {
        Ifx_print("test_can0_send_cpu0 !\r\n");
        can_send(0);
        vTaskDelay(5000);
    }
}

void test_can0_recv_cpu0(void *pvParameters)
{
    while (1)
    {
        xSemaphoreTake(g_can0_swamphore, portMAX_DELAY);
        Ifx_print("can0 recv can mesg ! ");
        Ifx_print("%x %x!\r\n", g_drv_can[0].rxdata[0], g_drv_can[0].rxdata[1]);
    }
}

void test_can2_send_cpu0(void *pvParameters)
{
    while (1)
    {
        Ifx_print("test_can2_send_cpu0 !\r\n");
        can_send(2);
        vTaskDelay(5000);
    }
}

void test_can2_recv_cpu0(void *pvParameters)
{
    while (1)
    {
        xSemaphoreTake(g_can2_swamphore, portMAX_DELAY);
        Ifx_print("can2 recv can mesg ! ");
        Ifx_print("%x %x!\r\n", g_drv_can[2].rxdata[0], g_drv_can[2].rxdata[1]);
    }
}

/************************************************************/
/**cpu1
/************************************************************/

void test_can0_send_cpu1(void *pvParameters)
{
    while (1)
    {
        //Ifx_print("test_can0_send_cpu0 !\r\n");
        can_send(0);
        vTaskDelay(20);
    }
}

void test_can0_recv_cpu1(void *pvParameters)
{
    int i = 0;
    while (1)
    {
        xSemaphoreTake(g_can0_swamphore, portMAX_DELAY);
        i++;
        if (i % 100 == 0)
        {
            IfxPort_togglePin(&MODULE_P13, 3);
        }
    }
}

void test_can2_send_cpu1(void *pvParameters)
{
    while (1)
    {
        can_send(2);
        vTaskDelay(20);
    }
}

void test_can2_recv_cpu1(void *pvParameters)
{
    int i = 0;
    while (1)
    {
        xSemaphoreTake(g_can2_swamphore, portMAX_DELAY);
        i++;
        if (i % 100 == 0)
        {
            IfxPort_togglePin(&MODULE_P13, 2);
        }
    }
}