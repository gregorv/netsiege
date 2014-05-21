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

#ifndef NETSIEGE_CLIENTSESSION_H
#define NETSIEGE_CLIENTSESSION_H

#include "udpconnection.h"
#include "network/network.pb.h"
#include <memory>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

namespace network {

class NetworkServer;

class ClientSession : public std::enable_shared_from_this<ClientSession>,
                      public UdpConnection<int, pb::C2SMessage, NetworkServer>
{
public:
    ClientSession(const udp::endpoint myEndpoint, NetworkServer* server);
    ~ClientSession();

    void inputPackage(const package_buffer_t& package);

private:
};

}

#endif // NETSIEGE_CLIENTSESSION_H
