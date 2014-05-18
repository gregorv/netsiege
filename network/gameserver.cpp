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

#include "gameserver.h"
#include <boost/asio.hpp>

using namespace boost::asio;
using boost::asio::ip::tcp;

GameServer::GameServer(const tcp::endpoint& interface)
 : m_ioservice(new io_service)
{
    try {
        m_acceptor = new tcp::acceptor(*m_ioservice, interface, true);
    }
    catch(std::exception& e) {
    }
}

GameServer::~GameServer()
{
    delete m_ioservice;
}
