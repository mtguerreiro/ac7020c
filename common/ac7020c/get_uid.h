
#ifndef GET_UID_H_
#define GET_UID_H_

#include "stdint.h"

#define GET_UID_ERROR_BUF_SIZE              -1
#define GET_UID_ERROR_QSPI_LOOK_UP_CFG      -2
#define GET_UID_ERROR_QSPI_INIT             -3
#define GET_UID_ERROR_QSPI_SELF_TEST        -4
#define GET_UID_ERROR_QSPI_SR3_READ         -5
#define GET_UID_ERROR_QSPI_UID_READ         -6

int32_t getUID(uint8_t *buffer, uint32_t size);

#endif /* GET_UID_H_ */
