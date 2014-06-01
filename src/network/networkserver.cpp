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

#include "networkserver.h"

#include "network/network.pb.h"
#include "debug/ndebug.h"
#include <oms/objectmanager.h>

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// using boost::asio;
// using boost::posix_time;

namespace network {

NetworkServer::NetworkServer(const udp::endpoint& interface)
: m_ioservice(), m_syncTimer(m_ioservice, boost::posix_time::seconds(SYNC_PERIOD)),
  m_callbackTimer(m_ioservice, boost::posix_time::seconds(2)), m_socket(m_ioservice, interface)
{
}

NetworkServer::~NetworkServer()
{
}

void NetworkServer::stopServer()
{
    m_ioservice.stop();
}

void NetworkServer::run()
{
    listen();
    sync();
    m_ioservice.run();
}

void NetworkServer::setObjectManager(std::shared_ptr< oms::ObjectManager > objMgr)
{
    m_objectManager = objMgr;
}

void NetworkServer::setTimeoutCallback(float timeout, NetworkServer::timeoutCallback_t callback)
{
    m_callbackTimerTimeout = timeout;
    m_callback = callback;
    m_callbackTimer.expires_from_now(boost::posix_time::seconds(m_callbackTimerTimeout));
    m_callbackTimer.async_wait(boost::bind(&NetworkServer::handle_timeoutCallback, this));
}

void NetworkServer::send(const udp::endpoint& remoteEndpoint, const package_buffer_t& package, size_t nBytes)
{
    m_socket.async_send_to(
        boost::asio::buffer(&package.front(),
                            std::min(package.size(), nBytes)),
        remoteEndpoint,
        boost::bind(&NetworkServer::handle_send, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred(),
            nBytes
        )
    );
}

void NetworkServer::listen()
{
    m_socket.async_receive_from(
        boost::asio::buffer(&m_receiveBuffer.front(),
                            m_receiveBuffer.size()),
        m_remoteEndpoint,
        boost::bind(&NetworkServer::handle_receive, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred()
        )
    );
}

void NetworkServer::syncTimeout()
{
    sync();
}

void NetworkServer::sync()
{
    if(m_objectManager.get() && m_clients.size() > 0) {
        auto msg = std::make_shared<pb::S2CMessage>();
        m_objectManager->serializeChanges(msg->mutable_world_state_update()->mutable_updated_objects());
        for(auto client: m_clients) {
            client.second->sendPackage(msg);
        }
    }
    m_syncTimer.expires_from_now(boost::posix_time::seconds(SYNC_PERIOD));
    m_syncTimer.async_wait(boost::bind(&NetworkServer::syncTimeout, this));
    closeDeadConnections();
}

void NetworkServer::handle_receive(const boost::system::error_code& error,
                                   std::size_t bytesTransferred)
{
    nDebug << "Received " << bytesTransferred << " bytes from " << m_remoteEndpoint << std::endl;
    auto it = m_clients.find(m_remoteEndpoint);
    if(it == m_clients.end()) {
        logInfo() << "Client connected from " << m_remoteEndpoint << std::endl;
        m_clients[m_remoteEndpoint] = std::make_shared<ClientSession>(m_remoteEndpoint, this);
        it = m_clients.find(m_remoteEndpoint);
    }
    it->second->inputPackage(m_receiveBuffer);
    listen();
}

void NetworkServer::handle_send(const boost::system::error_code& error, std::size_t bytesTransferred, std::size_t bytesExcpected)
{
    assert(bytesTransferred == bytesExcpected);
}

void NetworkServer::handle_timeoutCallback()
{
    m_callback();
    m_callbackTimer.expires_from_now(boost::posix_time::seconds(m_callbackTimerTimeout));
    m_callbackTimer.async_wait(boost::bind(&NetworkServer::handle_timeoutCallback, this));
}


void NetworkServer::closeDeadConnections()
{
    using namespace std::chrono;
    steady_time_point_t now = steady_clock::now();
    for(auto& it: m_clients) {
        duration<float> time_span = duration_cast<duration<float>>(now - it.second->timeOfLastAck());
        if(time_span.count() > CONNECTION_TIMEOUT) {
            logInfo() << "Client " << it.first << " timed out." << std::endl;
            it.second->close();
            m_clients.erase(it.first);
        }
    }
}



}

