
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "netinit.h"

/* Kernel */
#include "FreeRTOS.h"
#include "task.h"

/* Device and drivers */
#include "xparameters.h"
#include "netif/xadapter.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "xil_io.h"
#include "xqspips.h"

/* lwip */
#include "lwip/sockets.h"
#include "lwipopts.h"
#include "lwip/dhcp.h"

/* Logging config */
#include "clogging/logging_levels.h"

#ifndef LIBRARY_LOG_NAME
#define LIBRARY_LOG_NAME    "NET_INIT"
#endif

#ifndef LIBRARY_LOG_LEVEL
#define LIBRARY_LOG_LEVEL    LOG_DEBUG
#endif
#include "clogging/logging_stack.h"


#define QSPI_W25Q256JV_CMD_RD_SR3           0x15
#define QSPI_W25Q256JV_CMD_RD_UID           0x4B
#define QSPI_W25Q256JV_UID_SIZE_BYTES       8

#define QSPI_W25Q256JV_SR3_ADS_MASK         0x01
#define QSPI_W25Q256JV_SR3_ADS_3B_MASK      0

#define QSPI_W25Q256JV_UID_NDUMMY_ADS_3B    4
#define QSPI_W25Q256JV_UID_NDUMMY_ADS_4B    5
//=============================================================================

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================
/* Ethernet settings */
#define NETINIT_PLAT_EMAC_BASEADDR                  XPAR_XEMACPS_0_BASEADDR
#define NETINIT_CONFIG_THREAD_STACK_SIZE_DEFAULT    2048
#define NETINIT_CONFIG_THREAD_PRIO_DEFAULT          DEFAULT_THREAD_PRIO

typedef struct{
    /* Server netif */
    struct netif servernetif;

}netinitControl_t;
//=============================================================================


//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
netinitControl_t xnetinitControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void netinitGetId(void);

static void netinitRunNetworkTasks(void);

/**
 * @brief Initializes socket and updates DHCP timer.
 */
static void netinitNetworkThread(void *p);

/**
 * @brief Prints IP settings
 */
static void netinitPrintIPSettings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw);

extern volatile int dhcp_timoutcntr;
err_t dhcp_start(struct netif *netif);
void lwip_init();

//=============================================================================

//=============================================================================
/*---------------------------------- Task -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void netinit(void *param){

    (void) param;
    struct netif *netif;

    int mscnt = 0;

    netinitGetId();

    /* initialize lwIP before calling sys_thread_new */
    lwip_init();

    /* any thread using lwIP should be created using sys_thread_new */
    sys_thread_new(
        "netinitNWThread", netinitNetworkThread, NULL,
        NETINIT_CONFIG_THREAD_STACK_SIZE_DEFAULT,
        NETINIT_CONFIG_THREAD_PRIO_DEFAULT
    );

    netif = &xnetinitControl.servernetif;

    while (1) {

        vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);

        if (netif->ip_addr.addr) {
            LogInfo(( "DHCP request success" ));
            break;
        }

        mscnt += DHCP_FINE_TIMER_MSECS;

        if (mscnt >= DHCP_COARSE_TIMER_SECS * 2000) {
            LogError(( "DHCP request timed out. Configuring default ip of 192.168.0.10" ));
            IP4_ADDR(&(netif->ip_addr),  192, 168, 0, 10);
            IP4_ADDR(&(netif->netmask), 255, 255, 255,  0);
            IP4_ADDR(&(netif->gw),  192, 168, 0, 1);
            break;
        }
    }

    netinitPrintIPSettings(&(netif->ip_addr), &(netif->netmask), &(netif->gw));
    netinitRunNetworkTasks();

    vTaskDelete(NULL);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void netinitRunNetworkTasks(void){

    // sys_thread_new(
    //     "task_mqtt", taskMqtt,
    //     0,
    //     TASK_MQTT_CONFIG_TASK_STACK_SIZE,
    //     TASK_MQTT_CONFIG_TASK_PRIO
    // );
    //
    // sys_thread_new(
    //     "task_boost_inf_mng", taskBoostInfMqtt,
    //     0,
    //     TASK_BOOST_INF_MQTT_CONFIG_TASK_STACK_SIZE,
    //     TASK_BOOST_INF_MQTT_CONFIG_TASK_PRIO
    // );
}
//-----------------------------------------------------------------------------
static void netinitNetworkThread(void *p){

    (void)p;

    struct netif *netif;
    // uint32_t dnaLow, dnaHigh;

    /* The mac address of the board. this should be unique per board */
    unsigned char mac[6] = {0x00, 0x11, 0x13, 0x57, 0x3a, 0xf3};
    ip_addr_t ipaddr, netmask, gw;

    // dnaLow = Xil_In32(NETINIT_DNA_BASEADDR);
    // dnaHigh = Xil_In32(NETINIT_DNA_BASEADDR + 4);
    // mac[0] = (char)(  dnaHigh & 0xFF );
    // mac[1] = (char)( (dnaHigh & 0xFF00) >> 8 );
    // mac[2] = (char)( (dnaHigh & 0xFF0000) >> 16 );
    // mac[3] = (char)( (dnaHigh & 0xFF000000) >> 24 );
    // mac[4] = (char)(  dnaLow  & 0xFF );
    // mac[5] = (char)( (dnaLow  & 0xFF00) >> 8 );
    LogInfo((
        "Board MAC: %02X:%02X:%02X:%02X:%02X:%02X",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]
    ));

    int mscnt = 0;

    netif = &xnetinitControl.servernetif;

    LogInfo(( "Initializing network settings" ));

    ipaddr.addr = 0;
    gw.addr = 0;
    netmask.addr = 0;

    /* Add network interface to the netif_list, and set it as default */
    if (!xemac_add(netif, &ipaddr, &netmask, &gw, mac, NETINIT_PLAT_EMAC_BASEADDR)) {
        LogError(( "Error adding N/W interface" ));
        return;
    }

    netif_set_default(netif);

    /* specify that the network if is up */
    netif_set_up(netif);

    /* start packet receive thread - required for lwIP operation */
    sys_thread_new(
        "xemacif_input_thread", (void(*)(void*))xemacif_input_thread, netif,
                   NETINIT_CONFIG_THREAD_STACK_SIZE_DEFAULT,
                   NETINIT_CONFIG_THREAD_PRIO_DEFAULT
    );

    dhcp_start(netif);
    while (1) {
        vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);
        dhcp_fine_tmr();
        mscnt += DHCP_FINE_TIMER_MSECS;
        if (mscnt >= DHCP_COARSE_TIMER_SECS*1000) {
            dhcp_coarse_tmr();
            mscnt = 0;
        }
    }

    return;
}
//-----------------------------------------------------------------------------
static void netinitPrintIPSettings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw){

    LogInfo(( "Board IP : %d.%d.%d.%d", ip4_addr1(ip), ip4_addr2(ip), ip4_addr3(ip), ip4_addr4(ip) ));
    LogInfo(( "Netmask  : %d.%d.%d.%d", ip4_addr1(mask), ip4_addr2(mask), ip4_addr3(mask), ip4_addr4(mask) ));
    LogInfo(( "Gateway  : %d.%d.%d.%d", ip4_addr1(gw), ip4_addr2(gw), ip4_addr3(gw), ip4_addr4(gw) ));
}
//-----------------------------------------------------------------------------
static void netinitGetId(void){


    uint8_t id[QSPI_W25Q256JV_UID_SIZE_BYTES];
    uint8_t buffer[16] = {0};
    int status;
    uint32_t idOffset;
    uint32_t ads;
    static XQspiPs QspiInstance;
    XQspiPs_Config *QspiConfig;

    QspiConfig = XQspiPs_LookupConfig(XPAR_XQSPIPS_0_BASEADDR);
    if( QspiConfig == NULL ){
        LogError(("XQspiPs_LookupConfig failed"));
        while(1);
    }

    status = XQspiPs_CfgInitialize(&QspiInstance, QspiConfig, QspiConfig->BaseAddress);
    if( status == XST_DEVICE_IS_STARTED ){
        LogError(("Failed to initialize QSPI; it has been initialized already"));
        while(1);
    }

    status = XQspiPs_SelfTest(&QspiInstance);
    if (status != XST_SUCCESS) {
        LogError(("QSPI self test failed"));
        while(1);
    }

    XQspiPs_SetClkPrescaler(&QspiInstance, XQSPIPS_CLK_PRESCALE_8);
    XQspiPs_SetOptions(
        &QspiInstance,
        XQSPIPS_FORCE_SSELECT_OPTION | XQSPIPS_MANUAL_START_OPTION | XQSPIPS_HOLD_B_DRIVE_OPTION
    );

    XQspiPs_SetSlaveSelect(&QspiInstance);

    LogDebug(( "Reading SR-3" ));
    buffer[0] = QSPI_W25Q256JV_CMD_RD_SR3;
    status = XQspiPs_PolledTransfer(&QspiInstance, buffer, buffer, 2);
    if( status != XST_SUCCESS ){
        LogError(("Failed to transfer data to QSPI"));
        while(1);
    }
    LogDebug((
        "Buffer: %02x %02x",
        buffer[0], buffer[1]
    ));
    ads = buffer[1] & QSPI_W25Q256JV_SR3_ADS_MASK;

    LogDebug(( "Reading unique ID" ));
    buffer[0] = QSPI_W25Q256JV_CMD_RD_UID;
    status = XQspiPs_PolledTransfer(&QspiInstance, buffer, buffer, sizeof(buffer));
    if( status != XST_SUCCESS ){
        LogError(("Failed to transfer data to QSPI"));
        while(1);
    }

    if( ads == QSPI_W25Q256JV_SR3_ADS_3B_MASK )
        idOffset = 1 + QSPI_W25Q256JV_UID_NDUMMY_ADS_3B;
    else
        idOffset = 1 + QSPI_W25Q256JV_UID_NDUMMY_ADS_4B;

    memcpy(id, &buffer[idOffset], QSPI_W25Q256JV_UID_SIZE_BYTES);

    uint32_t k;
    printf("\r\nUID: ");
    for(k = 0; k < sizeof(id); k++){
        printf("%02x ", id[k]);
    }
    printf("\r\n");

    // LogDebug((
    //     "Buffer: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
    //     buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7],
    //     buffer[8], buffer[9], buffer[10], buffer[11], buffer[12]
    // ));
}
//-----------------------------------------------------------------------------
//=============================================================================
