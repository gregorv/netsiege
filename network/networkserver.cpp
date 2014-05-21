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

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

// using boost::asio;
// using boost::posix_time;

namespace network {

NetworkServer::NetworkServer(const udp::endpoint& interface)
: m_ioservice(), m_syncTimer(m_ioservice, boost::posix_time::seconds(5)),
  m_socket(m_ioservice, interface)
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

void NetworkServer::send(const udp::endpoint& remoteEndpoint, const package_buffer_t& package)
{
    m_socket.async_send_to(
        boost::asio::buffer(&package.front(),
                            package.size()),
        remoteEndpoint,
        boost::bind(&NetworkServer::handle_receive, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred()
//             package.size()
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
    std::cout << "Syncing clients..." << std::endl;
    m_syncTimer.expires_at(m_syncTimer.expires_at() + boost::posix_time::seconds(1));
    m_syncTimer.async_wait(boost::bind(&NetworkServer::syncTimeout, this));
    closeDeadConnections();
}

void NetworkServer::handle_receive(const boost::system::error_code& error,
                                   std::size_t bytesTransferred)
{
    std::cout << "Received " << bytesTransferred << " bytes from " << m_remoteEndpoint << std::endl;
    auto it = m_clients.find(m_remoteEndpoint);
    if(it == m_clients.end()) {
        m_clients[m_remoteEndpoint] = std::make_shared<ClientSession>(m_remoteEndpoint, this);
        it = m_clients.find(m_remoteEndpoint);
    }
    it->second->inputPackage(m_receiveBuffer);
    listen();
}

void NetworkServer::handle_send(const boost::system::error_code& error, std::size_t bytesTransferred, std::size_t bytesExcpected)
{

}

void NetworkServer::closeDeadConnections()
{
    using namespace std::chrono;
    steady_time_point_t now = steady_clock::now();
    for(auto& it: m_clients) {
        duration<float> time_span = duration_cast<duration<float>>(now - it.second->timeOfLastAck());
        if(time_span.count() > CONNECTION_TIMEOUT) {
            std::cout << "Client " << it.first << " timed out." << std::endl;
            it.second->close();
            m_clients.erase(it.first);
        }
    }
}



}

