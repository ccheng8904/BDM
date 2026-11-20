#include <stdint.h>
#include "protocol.h"

void DBGP_STS_callCallback(void)
{
	#ifdef DBGX_TEST_API_CALLBACK
    uint8_t TestDataForCallbackFunction[] = {1U, 2U, 3U};

    ///STS_logStringAndU32Hex("DBGP_ReceiveDataFunctionOfProt_CB: ", (uint32_t)DBGP_ReceiveDataFunctionOfProt_CB);
                     /* The physical addresses can move; for that testing output only for special cases */

    DBGP_ReceiveDataFunctionOfProt_CB(0x05, 0x39, TestDataForCallbackFunction, sizeof(TestDataForCallbackFunction));
           //- #define DBGX_MODULE_TOUCH_RW_MSG 0x0005; This ID is transparent here, because it comed from the PC.
	#endif	   
}

