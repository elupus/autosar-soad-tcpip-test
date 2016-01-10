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

#ifndef SOAD_CFG_H_
#define SOAD_CFG_H_

#include "Std_Types.h"

#define SOAD_CFG_ENABLE_DEVELOPMENT_ERROR STD_ON

#define SOAD_CFG_SOCKETROUTE_COUNT     2u
#define SOAD_CFG_PDUROUTE_COUNT        2u
#define SOAD_CFG_CONNECTIONGROUP_COUNT 2u
#define SOAD_CFG_CONNECTION_COUNT      2u

#define SOCKET_GRP1      0
#define SOCKET_GRP2      1

#define SOCKET_GRP1_CON1 0
#define SOCKET_GRP2_CON1 1

#define SOCKET_ROUTE1    0
#define SOCKET_ROUTE2    1

#endif /* SOAD_CFG_H_ */
