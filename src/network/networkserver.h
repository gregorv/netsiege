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

#ifndef NETSIEGE_NETWORKSERVER_H
#define NETSIEGE_NETWORKSERVER_H

#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <memory>
#include <map>

#include "network.h"
#include "clientsession.h"
#include "rpcdispatcher.h"

namespace oms {
    class ObjectManager;
}

namespace script {
    class ScriptEngine;
}

namespace network {

using boost::asio::ip::udp;

class NetworkServer : public RPCDispatcher
{
public:
    typedef std::function<void()> timeoutCallback_t;

    int RegisterNetworkSystem(std::shared_ptr<script::ScriptEngine> engine);

    NetworkServer(const udp::endpoint& interface);
    ~NetworkServer();

    void stopServer();
    void run();

    void setObjectManager(std::shared_ptr<oms::ObjectManager> objMgr);

    void send(const udp::endpoint& remoteEndpoint, const package_buffer_t& package, size_t nBytes);

    void setTimeoutCallback(float timeout, timeoutCallback_t callback);

    bool remoteProcedureCall(uint16_t client_id, std::shared_ptr< network::RPCPackage > package);

private:
    void syncTimeout();
    void sync();
    void listen();
    void handle_receive(const boost::system::error_code& error, std::size_t bytesTransferred);
    void handle_send(const boost::system::error_code& error, std::size_t bytesTransferred, std::size_t bytesExcpected);
    void handle_timeoutCallback();
    void handle_joinRequest(uint16_t client_id, std::shared_ptr< network::RPCPackage > package);
    void handle_joinAcknowledge(uint16_t client_id, std::shared_ptr< network::RPCPackage > package);

    void closeDeadConnections();

    size_t m_nextClientId = 0;
    boost::asio::io_service m_ioservice;
    boost::asio::deadline_timer m_syncTimer;
    boost::asio::deadline_timer m_callbackTimer;
    boost::asio::signal_set m_signals;
    timeoutCallback_t m_callback;
    float m_callbackTimerTimeout;
    udp::socket m_socket;
    udp::endpoint m_remoteEndpoint;
    package_buffer_t m_receiveBuffer;
    std::map<udp::endpoint, std::shared_ptr<ClientSession>> m_clients;
    std::shared_ptr<oms::ObjectManager> m_objectManager;
};

}

#endif // NETSIEGE_NETWORKSERVER_H
