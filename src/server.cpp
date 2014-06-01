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
#include "debug/ndebug.h"
#include "network/networkserver.h"
#include "campaign/manager.h"
#include "campaign/serverlogic.h"
#include "campaign/scriptfilemanager.h"
#include "oms/objectmanager.h"
#include <OGRE/OgreRoot.h>

int main(int argc, char **argv) {
    udp::endpoint listenInterface;
    boost::asio::ip::address addr(boost::asio::ip::address::from_string("0.0.0.0"));
    listenInterface.address(addr);
    listenInterface.port(6370);
    std::string mapName;
    if(argc == 4) {
        addr = boost::asio::ip::address::from_string(argv[2]);
        listenInterface.address(addr);
        listenInterface.port(atoi(argv[3]));
        mapName = argv[1];
    } else if(argc == 2) {
        mapName = argv[1];
    } else if(argc != 2) {
        std::cerr << argv[0] << " CampaignName (ListenIpV4 Port)" << std::endl;
        return -1;
    }
    network::NetworkServer server(listenInterface);

    Ogre::Root root;
    nDebug << "ResourceGroupManager Pointer: " << std::hex << Ogre::ResourceGroupManager::getSingletonPtr() << std::dec << std::endl;
    new campaign::ScriptFileManager;
    auto manager = std::make_shared<campaign::Manager>(mapName);
    if(!manager->loadCampaignPath()) {
        logError() << "Did not find campaign " << mapName << " in the specified search paths!" << std::endl;
        return -1;
    }
    auto objectManager = std::make_shared<oms::ObjectManager>();
    server.setObjectManager(objectManager);
    campaign::ServerLogic logic(manager);
    if(!logic.init()) {
        logError() << "Server logic could not be initialized, abort!" << std::endl;
        return -1;
    }
    server.setTimeoutCallback(0.05f, std::bind(&campaign::ServerLogic::step, &logic, 0.05f));
    server.run();
    delete Ogre::ResourceGroupManager::getSingleton()._getResourceManager("ScriptFile");
    return 0;
}
