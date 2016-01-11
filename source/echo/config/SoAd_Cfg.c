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

#include "SoAd.h"
#include "TcpIp.h"
#include "Catb.h"

extern void PduR_SoAdIfRxIndication(PduIdType id, const PduInfoType* info);

const SoAd_TpRxType suite_tp = {
        .rx_indication      = Catb_RxIndication,
        .copy_rx_data       = Catb_CopyRxData,
        .start_of_reception = Catb_StartOfReception,
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
                .upper      = &suite_tp,
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
