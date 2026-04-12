//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "get_uid.h"

#include "stdio.h"

/* Device and drivers */
#include "xparameters.h"
#include "xqspips.h"
//=============================================================================

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================
#define QSPI_W25Q256JV_CMD_RD_SR3           0x15
#define QSPI_W25Q256JV_CMD_RD_UID           0x4B
#define QSPI_W25Q256JV_UID_SIZE_BYTES       8

#define QSPI_W25Q256JV_SR3_ADS_MASK         0x01
#define QSPI_W25Q256JV_SR3_ADS_3B_MASK      0

#define QSPI_W25Q256JV_UID_NDUMMY_ADS_3B    4
#define QSPI_W25Q256JV_UID_NDUMMY_ADS_4B    5

#define QSPI_W25Q256JV_BUF_SIZE_BYTES       16
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*---------------------------------- Task -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t getUID(uint8_t *buffer, uint32_t size){

    int32_t status;
    uint8_t _buf[QSPI_W25Q256JV_BUF_SIZE_BYTES] = {0};

    uint32_t idOffset;
    uint32_t ads;
    static XQspiPs QspiInstance;
    XQspiPs_Config *QspiConfig = 0;

    if( size < QSPI_W25Q256JV_UID_SIZE_BYTES )
        return GET_UID_ERROR_BUF_SIZE;

    QspiConfig = XQspiPs_LookupConfig(XPAR_XQSPIPS_0_BASEADDR);
    if( QspiConfig == 0 )
        return GET_UID_ERROR_QSPI_LOOK_UP_CFG;

    status = XQspiPs_CfgInitialize(&QspiInstance, QspiConfig, QspiConfig->BaseAddress);
    if( status != XST_SUCCESS )
        return GET_UID_ERROR_QSPI_INIT;

    status = XQspiPs_SelfTest(&QspiInstance);
    if (status != XST_SUCCESS)
        return GET_UID_ERROR_QSPI_SELF_TEST;

    XQspiPs_SetClkPrescaler(&QspiInstance, XQSPIPS_CLK_PRESCALE_8);
    XQspiPs_SetOptions(
        &QspiInstance,
        XQSPIPS_FORCE_SSELECT_OPTION | XQSPIPS_MANUAL_START_OPTION | XQSPIPS_HOLD_B_DRIVE_OPTION
    );

    XQspiPs_SetSlaveSelect(&QspiInstance);

    _buf[0] = QSPI_W25Q256JV_CMD_RD_SR3;
    status = XQspiPs_PolledTransfer(&QspiInstance, _buf, _buf, 2);
    if (status != XST_SUCCESS)
        return GET_UID_ERROR_QSPI_SR3_READ;

    ads = _buf[1] & QSPI_W25Q256JV_SR3_ADS_MASK;

    _buf[0] = QSPI_W25Q256JV_CMD_RD_UID;
    status = XQspiPs_PolledTransfer(&QspiInstance, _buf, _buf, sizeof(_buf));
    if (status != XST_SUCCESS)
        return GET_UID_ERROR_QSPI_UID_READ;

    if( ads == QSPI_W25Q256JV_SR3_ADS_3B_MASK )
        idOffset = 1 + QSPI_W25Q256JV_UID_NDUMMY_ADS_3B;
    else
        idOffset = 1 + QSPI_W25Q256JV_UID_NDUMMY_ADS_4B;

    memcpy(buffer, &_buf[idOffset], QSPI_W25Q256JV_UID_SIZE_BYTES);

    return QSPI_W25Q256JV_UID_SIZE_BYTES;
}
//-----------------------------------------------------------------------------
//=============================================================================
