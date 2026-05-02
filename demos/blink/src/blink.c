
#include "stdio.h"

#include "xparameters.h"
#include "xgpio.h"
#include "sleep.h"
#include "xiltimer.h"

#include "xmulmv.h"

#include "mvops_data.h"

/* Logging config */
#include "clogging/logging_levels.h"

#ifndef LIBRARY_LOG_NAME
#define LIBRARY_LOG_NAME    "BLINK"
#endif

#ifndef LIBRARY_LOG_LEVEL
#define LIBRARY_LOG_LEVEL    LOG_DEBUG
#endif
#include "clogging/logging_stack.h"

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================
#define GPIO_LED_ADDR           XPAR_XGPIO_0_BASEADDR
#define GPIO_LED_CHANNEL        1
#define GPIO_LED                1
#define BLINK_PERIOD_MS         1000
//=============================================================================

//=============================================================================
/*------------------------------- Prototypes --------------------------------*/
//=============================================================================
static void mainTestHls(void);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static XGpio led;
static XMulmv mulmv;
//=============================================================================

//=============================================================================
/*---------------------------------- Main -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int main(void ){

    uint32_t state;

    mainTestHls();

    XGpio_Initialize(&led, GPIO_LED_ADDR);
    XGpio_SetDataDirection(&led, GPIO_LED_CHANNEL, 0);
    state = 0;

    while(1){
        state = state ^ 1;
        XGpio_DiscreteWrite(&led, GPIO_LED_CHANNEL, state);
        msleep(BLINK_PERIOD_MS);
    }
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
static void mainTestHls(void){

    XTime t1, t2;
    float t_exec;

    int status;
    float y[MV_DIM] = {0};

    LogInfo(( "Initializing mulmv..." ));
    status = XMulmv_Initialize(&mulmv, XPAR_MULMV_0_BASEADDR);
    if( status != XST_SUCCESS ){
        LogError(( "Failed to initialize mulmv." ));
    }

    LogInfo(( "Writing to m and v..." ));
    XMulmv_Write_m_Bytes(&mulmv, 0, (char *)m, sizeof(m));
    XMulmv_Write_v_Bytes(&mulmv, 0, (char *)v, sizeof(v));

    LogInfo(( "Starting and then waiting for mulmv..." ));
    XTime_GetTime(&t1);
    XMulmv_Start(&mulmv);
    while( XMulmv_IsDone(&mulmv) != 1 );
    XTime_GetTime(&t2);

    LogInfo(( "mulmv done. Reading result..." ));
    XMulmv_Read_y_Bytes(&mulmv, 0, (char *)y, sizeof(y));

    t_exec = ( (float) (t2 - t1) ) / ( (float) COUNTS_PER_SECOND ) / 1e-6;

    LogInfo(( "Last elements of result: %.4f, %.4f, %.4f", y[7], y[8], y[9] ));
    LogInfo(( "Execution time: %.4f us", t_exec ));
}
//=============================================================================
