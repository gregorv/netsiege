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

#ifndef NETSIEGE_NETWORKCLIENT_H
#define NETSIEGE_NETWORKCLIENT_H

#include "udpconnection.h"
#include "rpcdispatcher.h"

namespace network {
namespace pb {
    class S2CMessage;
}

// typedef std::shared_ptr<pb::S2CMessage> S2CMessage_ptr;

class NetworkClient : public UdpConnection<int, pb::S2CMessage, pb::C2SMessage, NetworkClient>,
                      public RPCDispatcher
{
public:
    NetworkClient(const udp::endpoint& serverEndpoint, const std::string& playerName);
    ~NetworkClient();

    int RegisterNetworkSystem(std::shared_ptr<script::ScriptEngine> engine);

    void initProcess();
    void run();
    void poll();
    void send(const udp::endpoint& remoteEndpoint, const package_buffer_t& package, size_t nBytes);

    void sendJoinRequest();

private:
    void listen();
    void handle_receive(const boost::system::error_code& error, std::size_t bytesTransferred);
    void handle_send(const boost::system::error_code& error, std::size_t bytesTransferred, std::size_t bytesExcpected);

    void handleJoinResponse(uint16_t client_id, std::shared_ptr< network::RPCPackage > package);

    bool remoteProcedureCall(uint16_t receiverClientId, std::shared_ptr<RPCPackage> package);

private:
    std::string m_name;
    boost::asio::io_service m_ioservice;
    udp::socket m_socket;
    package_buffer_t m_receiveBuffer;
    bool m_connectionActive;

};

}

#endif // NETSIEGE_NETWORKCLIENT_H
