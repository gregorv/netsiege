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

#include <iostream>
#include "network/networkserver.h"

int main(int argc, char **argv) {
    udp::endpoint listenInterface;
    boost::asio::ip::address addr(boost::asio::ip::address::from_string("0.0.0.0"));
    listenInterface.address(addr);
    listenInterface.port(6370);
    if(argc == 3) {
        addr = boost::asio::ip::address::from_string(argv[1]);
        listenInterface.address(addr);
        listenInterface.port(atoi(argv[2]));
    } else if(argc != 1) {
        std::cerr << argv[0] << "(ListenIpV4 Port)" << std::endl;
    }
    network::NetworkServer server(listenInterface);
    server.run();
    return 0;
}
