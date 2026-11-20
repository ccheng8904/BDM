#include <stdio.h>
#include <stdint.h>
#include "app_dbgx_test_api_callback.h"
#include "app_dbgx_test_api_callout.h"

int main(void)
{
    printf("### Main started.\n");

#ifdef STS_SIM_TEST_CI
    printf("*** OK: preprocessor switch STS_SIM_TEST_CI is defined\n\n");
#else
    printf("*** FAIL: preprocessor switch STS_SIM_TEST_CI is not defined\n");
#endif

#ifdef STS_SIM_TEST_CI
    printf("*** Test of API callbacks with data inputs from PC started.\n");
    APP_DBGX_test_API_CALLBACK();
    printf("*** Test of API callbacks with data inputs from PC finished.\n\n");

    printf("*** Test of data outputs from PC started.\n");
    APP_DBGX_test_API_CALLOUT();
    printf("*** Test of data outputs from PC finished.\n\n");

#endif
    printf("### Main finished.\n\n");

    return 0;
}

