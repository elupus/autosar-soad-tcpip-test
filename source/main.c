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
#include <unistd.h>
#include <stdlib.h>

extern TcpIp_ConfigType TcpIp_DefaultConfig;
extern SoAd_ConfigType  SoAd_DefaultConfig;

Std_ReturnType Det_ReportError(
        uint16 ModuleId,
        uint8 InstanceId,
        uint8 ApiId,
        uint8 ErrorId
    )
{
    fprintf(stderr, "error: %u, %u, %u, %u\n", ModuleId, InstanceId, ApiId, ErrorId);
    fflush(stderr);
    CU_FAIL_FATAL("Det_ReportError");
    return E_OK;
}

struct suite_pdu_state {
    uint32 rx_len;
};

struct suite_state {
    struct suite_pdu_state pdu[2];
};

struct suite_state suite_state;

void PduR_SoAdIfRxIndication(PduIdType id, const PduInfoType* info)
{
    suite_state.pdu[id].rx_len += info->SduLength;
}

int suite_init(void)
{
    memset(&suite_state, 0, sizeof(suite_state));
	TcpIp_Init(&TcpIp_DefaultConfig);
	TcpIp_RequestComMode(0u, TCPIP_STATE_ONLINE);
	SoAd_Init(&SoAd_DefaultConfig);
    return 0;
}

int suite_clean(void)
{
    TcpIp_RequestComMode(0u, TCPIP_STATE_OFFLINE);
	return 0;
}

void suite_tick(void)
{
    usleep(10000);
    TcpIp_MainFunction();
    SoAd_MainFunction();
}

void suite_tick_count(uint32 count)
{
    for(; count > 0u; --count) {
        suite_tick();
    }
}

void suite_startup(void)
{
    suite_tick_count(100u);
}

void suite_transmit_x(PduIdType tx, PduIdType rx)
{
    PduInfoType info;
    uint8       buf[100];
    struct suite_state sute_before = suite_state;
    info.SduDataPtr = buf;
    info.SduLength  = sizeof(buf);
    CU_ASSERT_EQUAL_FATAL(SoAd_IfTransmit(tx, &info), E_OK);

    suite_tick_count(10u);

    CU_ASSERT_EQUAL(suite_state.pdu[rx].rx_len, sute_before.pdu[rx].rx_len + sizeof(buf));
    CU_ASSERT_EQUAL(suite_state.pdu[tx].rx_len, sute_before.pdu[tx].rx_len);
}


void suite_transmit_0(void)
{
    suite_transmit_x(0, 1);
}

void suite_transmit_1(void)
{
    suite_transmit_x(1, 0);
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
    CU_add_test(suite, "transmit_1", suite_transmit_1);
    CU_add_test(suite, "transmit_0", suite_transmit_0);

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();
    return CU_get_error();
}
