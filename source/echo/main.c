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
#include "Catb.h"

#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

extern TcpIp_ConfigType TcpIp_DefaultConfig;
extern SoAd_ConfigType  SoAd_DefaultConfig;

uint8  Main_Buffer[1000u];
uint16 Main_Len;
uint16 Main_Tx;
uint16 Main_Rx;


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

BufReq_ReturnType Main_CatbStartOfReception(
    PduIdType               id,
    const PduInfoType*      info,
    PduLengthType           len,
    PduLengthType*          buf_len
)
{
    fprintf(stderr, "start: %u\n", id);
    *buf_len = sizeof(Main_Buffer);
    Main_Tx  = 0u;
    Main_Rx  = 0u;
    Main_Len = info->SduLength;
    return BUFREQ_OK;
}

BufReq_ReturnType Main_CatbCopyRxData(
    PduIdType               id,
    const PduInfoType*      info,
    PduLengthType*          buf_len
)
{
    fprintf(stderr, "copy: %u, %u\n", id, info->SduLength);
    for(int i = 0; i < info->SduLength; ++i) {
        fprintf(stdout, "%02x", info->SduDataPtr[i]);
    }
    memcpy(&Main_Buffer[Main_Rx], info->SduDataPtr, info->SduLength);
    Main_Rx += info->SduLength;
    if (Main_Len < Main_Rx) {
        Main_Len = Main_Rx;
    }
    *buf_len = sizeof(Main_Buffer) - Main_Rx;
    return BUFREQ_OK;
}

void Main_CatbRxIndication(
    PduIdType               id,
    Std_ReturnType          result
)
{
    fprintf(stderr, "rx: %u, %u\n", id, result);
    fprintf(stdout, "\n");
    fflush(stdout);
    if (result == E_OK) {
        Main_Tx = 0u;
        PduInfoType info;
        info.SduDataPtr = Main_Buffer;
        info.SduLength  = Main_Len;
        (void)Catb_Transmit(0u, &info);
    } else {
        Main_Len = 0u;
        Main_Tx  = 0u;
    }
}

BufReq_ReturnType Main_CatbCopyTxData(
        PduIdType               id,
        const PduInfoType*      info,
        RetryInfoType*          retry,
        PduLengthType*          available
    )
{
    if (info->SduLength <= Main_Len - Main_Tx) {
        memcpy(info->SduDataPtr, &Main_Buffer[Main_Tx], info->SduLength);
        Main_Tx += info->SduLength;
        *available = Main_Len - Main_Tx;
        return BUFREQ_OK;
    } else {
        return BUFREQ_E_NOT_OK;
    }
}

void Main_CatbTxConfirmation(
        PduIdType               id,
        Std_ReturnType          result
    )
{

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
