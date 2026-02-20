#include "pub.h"
#include "lwip_demo.h"


SemaphoreHandle_t g_eth_swamphore = NULL;


static struct netif xnetif_fos =
    {
        /* set MAC hardware address length */
        .hwaddr_len = (u8_t)ETH_HWADDR_LEN,

        /* set MAC hardware address */
        .hwaddr = {0x00, 0x20, 0x30, 0x40, 0x50, 0x60},

        /* maximum transfer unit */
        .mtu = 1500U,
};

void eth_link_monitor(void *pvParameters)
{
    while (1)
    {
        //Ifx_print("eth_link_monitor \r\n");
        xnetif_fos.link_callback(&xnetif_fos);
        vTaskDelay(5000);
    }
}

static void net_init_socket(void)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    IP4_ADDR(&ipaddr, 192, 168, 5, 123);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 5, 1);

    /* Create tcp_ip stack thread */
    tcpip_init(NULL, NULL);
    netif_add(&xnetif_fos, &ipaddr, &netmask, &gw, (void *)0, &ethernetif_init, tcpip_input);

    //netif_set_up(&xnetif_fos);
    //Ifx_print("eth_link_monitor will run %p \r\n", &xnetif_fos);
    xTaskCreate(eth_link_monitor, "eth_link_monitor", 256, NULL, 4, NULL);
    //Ifx_print("net_init_socket down \r\n");
}

void eth_init_fos(void *pvParameters)
{
    instruct_delay(2);
    Ifx_print("ret0 %x ret1 %x ret2 %x \r\n", ret0, ret1, ret2);
    net_init_socket(); // when test_eth_socket* use this
    ethernetif_recv(&xnetif_fos);
}

IFX_INTERRUPT(ISR_TxEth, 0, ISR_PRIORITY_TxEth);
IFX_INTERRUPT(ISR_RxEth, 0, ISR_PRIORITY_RxEth);
void ISR_TxEth(void)
{
    //	IfxGeth_Eth *eth = IfxEth_get();
    //
    //	if (IfxGeth_dma_isInterruptFlagSet(eth, IfxGeth_DmaChannel_0, IfxGeth_DmaInterruptFlag_transmitInterrupt) != FALSE)
    //    {
    //		IfxGeth_dma_clearInterruptFlag(eth, IfxGeth_DmaChannel_0, IfxGeth_DmaInterruptFlag_transmitInterrupt) ;
    //    }
}
void ISR_RxEth(void)
{
    BaseType_t xHigherPriorityTaskWoken;

    IfxGeth_Eth *eth = &g_drv_eth.eth;
    xHigherPriorityTaskWoken = pdFALSE;

    //IfxPort_togglePin(&MODULE_P13, 2);
    //IfxPort_togglePin(&MODULE_P33, 0);//开蜂鸣器

    if (g_eth_swamphore != NULL) //给出信号量
    {
        //IfxPort_setPinState(LED4, IfxPort_State_low);
        xSemaphoreGiveFromISR(g_eth_swamphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    //while (IfxGeth_Eth_isRxDataAvailable(eth, IfxGeth_RxDmaChannel_0) != 0) //检测是否收到数据包
    //if (IfxGeth_Eth_isRxDataAvailable(eth, IfxGeth_RxDmaChannel_0) != 0)
    {
    }
    //IfxPort_togglePin(&MODULE_P33, 0); //开蜂鸣器
    IfxGeth_dma_clearInterruptFlag(eth->gethSFR, IfxGeth_DmaChannel_0, IfxGeth_DmaInterruptFlag_receiveInterrupt);
    IfxGeth_dma_clearInterruptFlag(eth->gethSFR, IfxGeth_DmaChannel_0, IfxGeth_DmaInterruptFlag_normalInterruptSummary);
}

int isend = 0;
int ircv = 0;

void test_eth_socket_client_fos(void *pvParameters)
{
    int socket_descriptor = 0;
    struct sockaddr_in address;
    char data_test_send[500] = {1};
    struct ip_mreq multiCast;
    int i = 0;

    //vTaskDelay(5000);
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("192.168.5.196");
    address.sin_port = htons(8080);
    ////////////address.sin_port = htons(7667);
    Ifx_print("test_eth_socket_client_fos init \r\n");
    socket_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    multiCast.imr_multiaddr.s_addr = inet_addr("239.255.76.67");
    multiCast.imr_interface.s_addr = INADDR_ANY;
    setsockopt(socket_descriptor, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&multiCast, sizeof(multiCast));
    Ifx_print("test_eth_socket_client_fos down\r\n");

    for (;;)
    {
        //Ifx_print("send -- %d--msg \r\n", i++);
        i++;
        isend++;
        sendto(socket_descriptor, data_test_send, sizeof(data_test_send), 0, (struct sockaddr *)&address, sizeof(address));
        if (i % 200 == 0)
        {
            Ifx_print("send -- %d--msg \r\n", i++);
        }
        vTaskDelay(10000);
    }
}

void test_eth_socket_server_fos(void *pvParameters)
{
    int socket_descriptor = 0;
    struct sockaddr_in address;
    char data_test[128] = {0};
    int i = 0;
    err_t err;
    struct ip_mreq multiCast;

    Ifx_print("test_eth_socket_server_fos init \r\n");
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(7667);
    Ifx_print("test_eth_socket_server_fos init \r\n");
    socket_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    multiCast.imr_multiaddr.s_addr = inet_addr("239.255.76.67");
    multiCast.imr_interface.s_addr = INADDR_ANY;
    setsockopt(socket_descriptor, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&multiCast, sizeof(multiCast));

    err = bind(socket_descriptor, (struct sockaddr *)&address, sizeof(address));
    if (err != ERR_OK)
    {
        Ifx_print("bind error.%x \r\n", err);
        while (1)
        {
        }
    }
    Ifx_print("test_eth_socket_server_fos down\r\n");

    for (;;)
    {
        Ifx_print("recvdata start\r\n");
        err = recv(socket_descriptor, data_test, sizeof(data_test), 0);
        i++;
        ircv++;
        Ifx_print("data_test is %s\r\n", data_test);
        if (i % 20 == 0)
            Ifx_print("-%d -**-%d \r\n", ircv, isend);
        //vTaskDelay(1000);
    }
}
