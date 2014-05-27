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
#include "network/networkclient.h"

int main(int argc, char **argv) {
    if(argc != 3) {
        std::cerr << argv[0] << " ServerIpV4 Port" << std::endl;
        return -1;
    }
    udp::endpoint serverEndpoint;
    boost::asio::ip::address addr(boost::asio::ip::address_v4::from_string(argv[1]));
    serverEndpoint.address(addr);
    serverEndpoint.port(atoi(argv[2]));
    network::NetworkClient client(serverEndpoint, std::string("Serioux"));
    client.initProcess();
    client.run();
    return 0;
}
