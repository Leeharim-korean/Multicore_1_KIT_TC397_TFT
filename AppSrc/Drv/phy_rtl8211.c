/**
 * @file phy_rtl8211.c
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

#define PHY_ADDR 0x0

#define tc39x_phy_write IfxGeth_Phy_Clause22_writeMDIORegister
#define tc39x_phy_read IfxGeth_phy_Clause22_readMDIORegister

#define MII_BMCR 0x00        /* Basic mode control register */
#define MII_BMSR 0x01        /* Basic mode status register  */
#define MII_PHYSID1 0x02     /* PHYS ID 1                   */
#define MII_PHYSID2 0x03     /* PHYS ID 2                   */
#define MII_ADVERTISE 0x04   /* Advertisement control reg   */
#define MII_LPA 0x05         /* Link partner ability reg    */
#define MII_EXPANSION 0x06   /* Expansion register          */
#define MII_CTRL1000 0x09    /* 1000BASE-T control          */
#define MII_STAT1000 0x0a    /* 1000BASE-T status           */
#define MII_MMD_CTRL 0x0d    /* MMD Access Control Register */
#define MII_MMD_DATA 0x0e    /* MMD Access Data Register */
#define MII_ESTATUS 0x0f     /* Extended Status             */
#define MII_DCOUNTER 0x12    /* Disconnect counter          */
#define MII_FCSCOUNTER 0x13  /* False carrier counter       */
#define MII_NWAYTEST 0x14    /* N-way auto-neg test reg     */
#define MII_RERRCOUNTER 0x15 /* Receive error counter       */
#define MII_SREVISION 0x16   /* Silicon revision            */
#define MII_RESV1 0x17       /* Reserved...                 */
#define MII_LBRERROR 0x18    /* Lpback, rx, bypass error    */
#define MII_PHYADDR 0x19     /* PHY address                 */
#define MII_RESV2 0x1a       /* Reserved...                 */
#define MII_TPISTATUS 0x1b   /* TPI status for 10mbps       */
#define MII_NCONFIG 0x1c     /* Network interface config    */

/* Basic mode control register. */
#define BMCR_RESV 0x003f      /* Unused...                   */
#define BMCR_SPEED1000 0x0040 /* MSB of Speed (1000)         */
#define BMCR_CTST 0x0080      /* Collision test              */
#define BMCR_FULLDPLX 0x0100  /* Full duplex                 */
#define BMCR_ANRESTART 0x0200 /* Auto negotiation restart    */
#define BMCR_ISOLATE 0x0400   /* Isolate data paths from MII */
#define BMCR_PDOWN 0x0800     /* Enable low power state      */
#define BMCR_ANENABLE 0x1000  /* Enable auto negotiation     */
#define BMCR_SPEED100 0x2000  /* Select 100Mbps              */
#define BMCR_LOOPBACK 0x4000  /* TXD loopback bits           */
#define BMCR_RESET 0x8000     /* Reset to default state      */
#define BMCR_SPEED10 0x0000   /* Select 10Mbps               */

/* Basic mode status register. */
#define BMSR_ERCAP 0x0001        /* Ext-reg capability          */
#define BMSR_JCD 0x0002          /* Jabber detected             */
#define BMSR_LSTATUS 0x0004      /* Link status                 */
#define BMSR_ANEGCAPABLE 0x0008  /* Able to do auto-negotiation */
#define BMSR_RFAULT 0x0010       /* Remote fault detected       */
#define BMSR_ANEGCOMPLETE 0x0020 /* Auto-negotiation complete   */
#define BMSR_RESV 0x00c0         /* Unused...                   */
#define BMSR_ESTATEN 0x0100      /* Extended Status in R15      */
#define BMSR_100HALF2 0x0200     /* Can do 100BASE-T2 HDX       */
#define BMSR_100FULL2 0x0400     /* Can do 100BASE-T2 FDX       */
#define BMSR_10HALF 0x0800       /* Can do 10mbps, half-duplex  */
#define BMSR_10FULL 0x1000       /* Can do 10mbps, full-duplex  */
#define BMSR_100HALF 0x2000      /* Can do 100mbps, half-duplex */
#define BMSR_100FULL 0x4000      /* Can do 100mbps, full-duplex */
#define BMSR_100BASE4 0x8000     /* Can do 100mbps, 4k packets  */

#define MIIM_RTL8211F_PAGE_SELECT 0x1f
#define MIIM_RTL8211F_TX_DELAY 0x100
#define MIIM_RTL8211F_LCR 0x10

/* RTL8211F PHY Status Register */
#define MIIM_RTL8211F_PHY_STATUS 0x1a
#define MIIM_RTL8211F_AUTONEG_ENABLE 0x1000
#define MIIM_RTL8211F_PHYSTAT_SPEED 0x0030
#define MIIM_RTL8211F_PHYSTAT_GBIT 0x0020
#define MIIM_RTL8211F_PHYSTAT_100 0x0010
#define MIIM_RTL8211F_PHYSTAT_DUPLEX 0x0008
#define MIIM_RTL8211F_PHYSTAT_SPDDONE 0x0800
#define MIIM_RTL8211F_PHYSTAT_LINK 0x0004

#define PHY_AUTONEGOTIATE_TIMEOUT 5000

#define RTL8211FI_MDIO_BMCR 0x00

#define RTL8211FI_MDIO_BMSR 0x01

#define RTL8211FI_MDIO_ANAR 0x04

#define RTL8211FI_MDIO_MDIO_GBCR 0x09

#define RTL8211FI_MDIO_LED 0x1F

#define RTL8211FI_Page_LCR 0x10

#define RTL8211FI_Page_EEELCR 0x11

/**
 * @brief 初始化8211f
 * 
 * @return int 
 */
int rtl8211f_init_config()
{

    Ifx_print("Phy_Addr is %d Init 1\r\n", PHY_ADDR);
    // reset PHY
    IfxGeth_Phy_Clause22_writeMDIORegister(0, RTL8211FI_MDIO_BMCR, 0x8000); // reset
    uint32 value;
    do
    {
        IfxGeth_phy_Clause22_readMDIORegister(0, RTL8211FI_MDIO_BMCR, &value);
    } while (value & 0x8000); // wait for reset to finish
    instruct_delay(2);
    Ifx_print("Phy_Addr is %d Init 2\r\n", PHY_ADDR);
    // setup PHY
    IfxGeth_Phy_Clause22_writeMDIORegister(0, RTL8211FI_MDIO_MDIO_GBCR, 0x0200); // advertise 1000BASE-T (full/half duplex) restart 0x0200
    IfxGeth_Phy_Clause22_writeMDIORegister(0, RTL8211FI_MDIO_ANAR, 0x0101);      // advertise 100BASE-TX full duplex only
    IfxGeth_Phy_Clause22_writeMDIORegister(0, RTL8211FI_MDIO_BMCR, 0x1140);      // 1000M,enable auto-negotiation, restart auto-negotiation 0X1140
    Ifx_print("Phy_Addr is %d Init 3\r\n", PHY_ADDR);
    /* we set our LEDs */
    IfxGeth_Phy_Clause22_writeMDIORegister(0, RTL8211FI_MDIO_LED, 0x0d04);
    IfxGeth_Phy_Clause22_writeMDIORegister(0, RTL8211FI_Page_LCR, 0x6308);
    IfxGeth_Phy_Clause22_writeMDIORegister(0, RTL8211FI_Page_EEELCR, 0x000C);
    IfxGeth_Phy_Clause22_writeMDIORegister(0, RTL8211FI_MDIO_LED, 0x0000);
    //RTL8211FI_PhyInitDone = 1;
}

/**
 * @brief 获取8211f的状态
 * 
 * @return int 
 */

int rtl8211f_parse_status()
{
    uint32 speed = 0;
    uint32 mii_reg = 0;
    int i = 0;

    tc39x_phy_write(PHY_ADDR, MIIM_RTL8211F_PAGE_SELECT, 0xa43);
    tc39x_phy_read(PHY_ADDR, MIIM_RTL8211F_PHY_STATUS, &mii_reg);

    while (!(mii_reg & MIIM_RTL8211F_PHYSTAT_LINK))
    {
        if (i > PHY_AUTONEGOTIATE_TIMEOUT)
        {
            Ifx_print(" TIMEOUT !\r\n");
            break;
        }

        if ((i++ % 1000) == 0)
            Ifx_print('.');
        //wait(TimeConst_100us);
        tc39x_phy_read(PHY_ADDR, MIIM_RTL8211F_PHY_STATUS, &mii_reg);
    }
    Ifx_print("\r\n");
    Ifx_print("Rtl8211f Link is Ok !\r\n");
    if (mii_reg & MIIM_RTL8211F_PHYSTAT_DUPLEX)
        Ifx_print("Rtl8211f is duplex full !\r\n");
    else
        Ifx_print("Rtl8211f is duplex half !\r\n");

    speed = (mii_reg & MIIM_RTL8211F_PHYSTAT_SPEED);

    switch (speed)
    {
    case MIIM_RTL8211F_PHYSTAT_GBIT:
        Ifx_print("Rtl8211f is speed 1000m!\r\n");
        break;
    case MIIM_RTL8211F_PHYSTAT_100:
        Ifx_print("Rtl8211f is speed 100m!\r\n");
        break;
    default:
        Ifx_print("Rtl8211f is speed 10m!\r\n");
    }
    /* restore to default page 0 */
    tc39x_phy_write(PHY_ADDR, MIIM_RTL8211F_PAGE_SELECT, 0x0);
    return 0;
}

boolean rtl8211f_link(void)
{
    boolean linkEstablished = FALSE;
    uint32 value;
    Ifx_print("rtl8211f_link 1\r\n");
    do
    {
        IfxGeth_phy_Clause22_readMDIORegister(0, RTL8211FI_MDIO_BMSR, &value);
        linkEstablished = ((value & (1 << 2)) != 0) ? TRUE : FALSE;
    } while (!linkEstablished);

    return linkEstablished;
}

boolean rtl8211f_link_state(void)
{
    boolean linkEstablished = FALSE;
    uint32 value;
    //Ifx_print("rtl8211f_link_state 1\r\n");

    IfxGeth_phy_Clause22_readMDIORegister(0, RTL8211FI_MDIO_BMSR, &value);
    linkEstablished = ((value & (1 << 2)) != 0) ? TRUE : FALSE;

    return linkEstablished;
}
