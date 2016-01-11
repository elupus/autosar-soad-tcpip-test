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

#include "SoAd_Cfg.h"
#include "SoAd.h"
#include "TcpIp.h"

extern void PduR_SoAdIfRxIndication(PduIdType id, const PduInfoType* info);

static BufReq_ReturnType PduR_SoAdIfStartOfReception(
        PduIdType               id,
        const PduInfoType*      info,
        PduLengthType           len,
        PduLengthType*          buf_len
    )
{
    *buf_len = (PduLengthType)0xffffu;
    return BUFREQ_OK;
}

static BufReq_ReturnType PduR_SoAdIfCopyRxData(
        PduIdType               id,
        const PduInfoType*      info,
        PduLengthType*          buf_len
    )
{
    if (info->SduLength) {
        PduR_SoAdIfRxIndication(id, info);
    }
    *buf_len = (PduLengthType)0xffffu;
    return BUFREQ_OK;
}

static void PduR_SoAdTpRxIndication_If(
        PduIdType               id,
        Std_ReturnType          result
    )
{
}

const SoAd_TpRxType suite_if = {
        .rx_indication      = PduR_SoAdTpRxIndication_If,
        .copy_rx_data       = PduR_SoAdIfCopyRxData,
        .start_of_reception = PduR_SoAdIfStartOfReception,
};

const TcpIp_SockAddrInetType socket_remote_any_v4 = {
    .domain  = TCPIP_AF_INET,
    .addr[0] = TCPIP_IPADDR_ANY,
    .port    = TCPIP_PORT_ANY,
};

const SoAd_SoGrpConfigType           socket_group_1 = {
    .localport = 8000,
    .localaddr = TCPIP_LOCALADDRID_ANY,
    .domain    = TCPIP_AF_INET,
    .protocol  = TCPIP_IPPROTO_TCP,
    .automatic = TRUE,
    .initiate  = FALSE,
    .socket_route_id = SOCKET_ROUTE1
};

const SoAd_SocketRouteType           socket_route_1 = {
        .header_id = SOAD_PDUHEADERID_INVALID,
        .destination = {
                .upper      = &suite_if,
                .pdu        = 0u
        }
};

const SoAd_SoConConfigType           socket_group_1_conn_1 = {
    .group  = SOCKET_GRP1,
    .remote = (const TcpIp_SockAddrType*)&socket_remote_any_v4,
    .socket_route_id  = SOAD_SOCKETROUTEID_INVALID,
};

const SoAd_SoConConfigType           socket_group_1_conn_2 = {
    .group  = SOCKET_GRP1,
    .remote = (const TcpIp_SockAddrType*)&socket_remote_any_v4,
    .socket_route_id  = SOAD_SOCKETROUTEID_INVALID,
};

const SoAd_PduRouteType              pdu_route_1 = {
        .destination = {
                .header_id  = SOAD_PDUHEADERID_INVALID,
                .connection = SOCKET_GRP1_CON1,
        },
};

const SoAd_ConfigType SoAd_DefaultConfig = {
    .groups = {
        [SOCKET_GRP1] = &socket_group_1,
    },

    .connections = {
        [SOCKET_GRP1_CON1] = &socket_group_1_conn_1,
        [SOCKET_GRP2_CON1] = &socket_group_1_conn_2,
    },

    .socket_routes     = {
        [SOCKET_ROUTE1] = &socket_route_1,
    },

    .pdu_routes        = {
        &pdu_route_1,
    },
};
