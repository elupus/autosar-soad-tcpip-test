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
#include "Det.h"

#include <stdio.h>
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
    return E_OK;
}

void PduR_SoAdIfRxIndication(PduIdType id, const PduInfoType* info)
{
    fwrite(info->SduDataPtr, 1u, info->SduLength, stdout);
    SoAd_IfTransmit(id, info);
}

int main(void)
{
    TcpIp_Init(&TcpIp_DefaultConfig);
    SoAd_Init(&SoAd_DefaultConfig);

    TcpIp_RequestComMode(0u, TCPIP_STATE_ONLINE);

    while (TRUE) {
        usleep(10000);
        TcpIp_MainFunction();
        SoAd_MainFunction();
    }

    return 0u;
}
