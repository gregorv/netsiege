/*
 * netsiege - A multiplayer point and click adventure
 * Copyright (C) 2014 Gregor Vollmer <gregor@celement.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NETSIEGE_NETWORK_H
#define NETSIEGE_NETWORK_H

#include <chrono>
#include <array>

namespace network {

// Maximum size of a network UDP package in bytes. Small enough to be transmitted
// with IP, but so large that it will be fragmented =(
const std::size_t MAX_PACKAGE_SIZE = 32000;

// Maximum size of payload in an RPC call.
const std::size_t MAX_RPC_DATA_SIZE = 512;

// When CONNECTION_TIMEOUT seconds have passed without communication from
// a partner, the connection is considered lost.
const float CONNECTION_TIMEOUT = 10.0f;

// If no ack arives for a package after PACKAGE_LOSS_TIMEOUT seconds,
// the package is considered lost.
const float PACKAGE_LOSS_TIMEOUT = 0.3f;

// The protocol version used by the game.
// Considered in range of 0..31 (5-bit)
const uint8_t PROTOCOL_VERSION = 15;

// The "unique" 11bit protocol base ID.
//  python -c "import random; print(hex(0xFFFF & (random.randint(0, 2**16) << 5)))"
const uint16_t PROTOCOL_BASE_ID = 0xcd40;

// The protocol ID as transmitted in the header of every network package.
// It consists of the 11bit "unique" base ID and the 5bit protocol
// version number.
const uint16_t PROTOCOL_ID = PROTOCOL_BASE_ID + PROTOCOL_VERSION;

// Time passing between state pushs to connected clients
const float SYNC_PERIOD = 0.1f;

typedef std::array<char, MAX_PACKAGE_SIZE> package_buffer_t;
typedef std::array<char, MAX_RPC_DATA_SIZE> rpc_data_t;
typedef std::chrono::time_point<std::chrono::steady_clock> steady_time_point_t;
typedef std::chrono::duration<std::chrono::steady_clock> steady_time_duration_t;

typedef uint16_t seq_id_t;
const seq_id_t MAX_SEQ_ID = 0xFFFF;

typedef uint16_t rpc_id_t;
const rpc_id_t MAX_RPC_ID = 0xFFFF;

// The RPC IDs up to this constant are reserved for interal use by the C++-side
// of the game. Registering an AngelScript handler for these RPCs is invalid
// and will result in an error!
const rpc_id_t MAX_INTERNAL_RPC_ID = 15;

const rpc_id_t RPC_ID_JOIN_SERVER_REQ = 0;
// RPC Arguments: uint32 client_version, string name
static const char* RPC_ARGS_JOIN_SERVER_REQ = "iL";

const rpc_id_t RPC_ID_JOIN_SERVER_RESP = 1;
// RPC arguments: uint32 player_id, uint32 server_version, char player_accepted, string map_name, uint32 map_version
static const char* RPC_ARGS_JOIN_SERVER_RESP = "iicLi";

const rpc_id_t RPC_ID_JOIN_SERVER_ACK = 2;
// RPC arguments: char accept
static const char* RPC_ARGS_JOIN_SERVER_ACK = "c";

typedef uint16_t ack_mask_t;
const uint16_t NUM_ACKS = sizeof(ack_mask_t)*8;

bool isSeqIdMoreRecent(seq_id_t seqId1, seq_id_t seqId2);
seq_id_t seqIdDifference(seq_id_t seqId1, seq_id_t seqId2);
bool isSeqIdAcknowledged(ack_mask_t ackMask, seq_id_t remoteSeqId, seq_id_t seqId);

}

#endif//NETSIEGE_NETWORK_H
