#ifdef STS_SIM_TEST_CI

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sts_sim_test_uty.h"
#include "protocol_sts.h"
#include "debug_extended/protocol.h"
#include "debug_extended/api_cfg.h"
#include "app_dbgx_test_api_callback.h"

#ifdef DBGX_BIN_DAT_FUNCTIONS_ENABLED
static void APP_DBGX_receiveData(uint16_t Function, uint8_t *Data, uint16_t DataLen);

static void APP_DBGX_receiveData(uint16_t Function, uint8_t *Data, uint16_t DataLen)
{
    STS_logStringAndU16Hex("Function: ", Function);
    STS_logStringAndU8ArrayHex("Data:     ", Data, DataLen);
    STS_logStringAndU32Hex("DataLen:  ", DataLen);
}
#endif

void APP_DBGX_test_API_CALLBACK(void)
{
#ifdef DBGX_BIN_DAT_FUNCTIONS_ENABLED
    int32_t RetFailStatus;
#endif

#if defined(DBGX_TEXT_DAT_FUNCTIONS_ENABLED) || defined(DBGX_BIN_DAT_FUNCTIONS_ENABLED) || defined(DBGX_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED)
    STS_logString("\n=== DBGX_init();");
    DBGX_init();
#endif

#ifdef DBGX_BIN_DAT_FUNCTIONS_ENABLED
    STS_logString("\n=== RetFailStatus = DBGX_notifyReceiveData_CB(APP_DBGX_receiveData, DBGX_MODULE_TOUCH_RW_MSG);");
    RetFailStatus = DBGX_notifyReceiveData_CB(APP_DBGX_receiveData, DBGX_MODULE_TOUCH_RW_MSG);
    STS_logStringAndU32Hex("DBGX_notifyReceiveData_CB: returns ", RetFailStatus);

    STS_logString("\n=== DBGP_STS_callCallback();");
    DBGP_STS_callCallback();
#endif
}

#endif /* From: #ifdef STS_SIM_TEST_CI */

