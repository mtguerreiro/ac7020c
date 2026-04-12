//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include <stdint.h>
#include <stdio.h>

/* Tasks */
#include "blink.h"
#include "netinit.h"
#include "tcpecho.h"

/* Kernal */
#include "FreeRTOS.h"
#include "task.h"

#include "netif/xadapter.h"

#include "get_uid.h"

/* Logging config */
#include "clogging/logging_levels.h"

#ifndef LIBRARY_LOG_NAME
#define LIBRARY_LOG_NAME    "MAIN"
#endif

#ifndef LIBRARY_LOG_LEVEL
#define LIBRARY_LOG_LEVEL    LOG_DEBUG
#endif
#include "clogging/logging_stack.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int32_t mainGetMAC(uint8_t *mac);
static void mainOnNetworkInit(void);
//=============================================================================

//=============================================================================
/*---------------------------------- Main -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int main(void){

    netinitParams_t netinitCfg;
    int32_t status;
    uint8_t mac[6];

    status = mainGetMAC(mac);
    netinitCfg.mac = status < 0 ? 0 : mac;
    netinitCfg.onInit = mainOnNetworkInit;

    sys_thread_new(
        "blink_thrd", blink,
        0,
        BLINK_CONFIG_TASK_STACK_SIZE,
        BLINK_CONFIG_TASK_PRIO
    );

    sys_thread_new(
        "netinit", netinit,
        (void *)&netinitCfg,
        NETINIT_CONFIG_TASK_STACK_SIZE,
        NETINIT_CONFIG_TASK_PRIO
    );

    vTaskStartScheduler();
    while(1);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t mainGetMAC(uint8_t *mac){

    uint32_t k;
    int32_t status;
    uint8_t buffer[32] = {0};

    status = getUID(buffer, sizeof(buffer));

    if( status < 0 ){
        LogError(( "Failed to read UID with status %d", (unsigned int)status ));
        return status;
    }

    for(k = 0; k < 6; k++)
        mac[k] = buffer[k];

    /* Set unicast and locally administred flags*/
    mac[0] = (mac[0] & 0xFE) | 0x02;

    return 0;
}
//-----------------------------------------------------------------------------
static void mainOnNetworkInit(void){

    sys_thread_new(
        "tcp_echo", tcpecho,
        0,
        TCP_ECHO_CONFIG_TASK_STACK_SIZE,
        TCP_ECHO_CONFIG_TASK_PRIO
    );
}
//-----------------------------------------------------------------------------
//=============================================================================
