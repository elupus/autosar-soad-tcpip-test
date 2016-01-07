/* Copyright (C) 2015 Joakim Plate
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "TcpIp.h"
#include "SoAd.h"

#include "CUnit/Basic.h"
#include "CUnit/Automated.h"
#include <arpa/inet.h>
#include <netdb.h>

extern TcpIp_ConfigType TcpIp_DefaultConfig;
extern SoAd_ConfigType  SoAd_DefaultConfig;

Std_ReturnType Det_ReportError(
        uint16 ModuleId,
        uint8 InstanceId,
        uint8 ApiId,
        uint8 ErrorId
    )
{
    return E_OK;
}

void PduR_SoAdIfRxIndication(PduIdType id, const PduInfoType* info)
{

}

int suite_init(void)
{
	TcpIp_Init(&TcpIp_DefaultConfig);
	SoAd_Init(&SoAd_DefaultConfig);
    return 0;
}

int suite_clean(void)
{
	return 0;
}

void suite_tick(void)
{
    usleep(10000);
    TcpIp_MainFunction();
    SoAd_MainFunction();
}

void suite_startup(void)
{
    suite_tick();
    suite_tick();
    suite_tick();
    suite_tick();
}

void suite_transmit_0(void)
{
    PduInfoType info;
    uint8       buf[100];
    info.SduDataPtr = buf;
    info.SduLength  = sizeof(buf);
    CU_ASSERT_EQUAL(SoAd_IfTransmit(0, &info), E_OK);
}

int main(void)
{
    CU_pSuite suite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

    /* add a suite to the registry */
    suite = CU_add_suite("Suite_Generic V4", suite_init, suite_clean);

    CU_add_test(suite, "startup"   , suite_startup);
    CU_add_test(suite, "transmit_0", suite_transmit_0);

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    /* Run results and output to files */
    CU_automated_run_tests();
    CU_list_tests_to_file();

    CU_cleanup_registry();
    return CU_get_error();
}
