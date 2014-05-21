/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
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

#include "clientsession.h"
#include "network/network.pb.h"

namespace network {

ClientSession::ClientSession(const udp::endpoint myEndpoint, network::NetworkServer* server)
: UdpConnection(myEndpoint, server)
{

}

ClientSession::~ClientSession()
{

}

void ClientSession::inputPackage(const package_buffer_t& package)
{
    pb::C2SMessage msg;
    msg.ParseFromArray(&package.front(), package.size());
    refresh(msg.seq_id(), msg.ack_seq_id(), msg.ack_mask());
}

}
