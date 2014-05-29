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

#include "networkclient.h"
#include <boost/bind.hpp>

namespace network {

NetworkClient::NetworkClient(const udp::endpoint& serverEndpoint, const std::string& playerName)
: UdpConnection(serverEndpoint, this), m_name(playerName),
m_socket(m_ioservice, udp::endpoint(udp::v4(), 0))
{

}

NetworkClient::~NetworkClient()
{

}

void NetworkClient::listen()
{
    m_socket.async_receive_from(
        boost::asio::buffer(&m_receiveBuffer.front(),
                            m_receiveBuffer.size()),
        m_remoteEndpoint,
        boost::bind(&NetworkClient::handle_receive, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred()
        )
    );
}

void NetworkClient::send(const udp::endpoint& remoteEndpoint, const package_buffer_t& package, size_t nBytes)
{
    package.front();
    m_socket.async_send_to(
        boost::asio::buffer(&package.front(),
                            std::min(package.size(), nBytes)),
        remoteEndpoint,
        boost::bind(&NetworkClient::handle_receive, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred()
//             package.size()
            )
        );
}

void NetworkClient::initProcess()
{
    listen();
}

void NetworkClient::poll()
{
    m_ioservice.poll();
}

void NetworkClient::run()
{
    m_ioservice.run();
}

void NetworkClient::handle_receive(const boost::system::error_code& error,
                                   std::size_t bytesTransferred)
{
    pb::S2CMessage msg;
    msg.ParseFromArray(&m_receiveBuffer.front(),
                       m_receiveBuffer.size());
    if(!parsePackage(msg)) {
    }
    listen();
}

void NetworkClient::handle_send(const boost::system::error_code& error, std::size_t bytesTransferred, std::size_t bytesExcpected)
{

}


}
