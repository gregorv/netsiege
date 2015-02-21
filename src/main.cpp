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
#include "script/scriptengine.h"
#include "script/scriptfilemanager.h"
#include "campaign/manager.h"
#include "campaign/clientlogic.h"
#include "debug/ndebug.h"
#include <Ogre.h>

int main(int argc, char **argv) {

    if(argc != 3) {
        std::cerr << argv[0] << " ServerIpV4 Port" << std::endl;
        return -1;
    }
    udp::endpoint serverEndpoint;
    nDebug << "IP address:" << argv[1];
    boost::asio::ip::address addr(boost::asio::ip::address::from_string(argv[1]));
    serverEndpoint.address(addr);
    serverEndpoint.port(atoi(argv[2]));
    Ogre::Root root;
    nDebug << "ResourceGroupManager Pointer: " << std::hex << Ogre::ResourceGroupManager::getSingletonPtr() << std::dec << std::endl;
    new script::ScriptFileManager;
    auto scriptEngine = std::make_shared<script::ScriptEngine>();
    network::NetworkClient client(serverEndpoint, std::string("Serioux"));
    std::shared_ptr<campaign::Manager> campaignManager;
    std::shared_ptr<campaign::ClientLogic> logic;
    client.setJoinAcceptHandler([&campaignManager,&scriptEngine,&logic,&client](uint32_t player_id, uint32_t server_version, bool accepted, std::string map, uint32_t map_version) {
        nDebug << "Load map '" << map << "'" << std::endl;
        campaignManager = std::make_shared<campaign::Manager>(map, scriptEngine);
        client.setObjectManager(campaignManager->objectManager());
        if(!campaignManager->loadCampaignPath()) {
            logError() << "Did not find campaign " << map << " in the specified search paths!" << std::endl;
            return false;
        }
        client.loadRpcHandlerSpec();
        logic = std::make_shared<campaign::ClientLogic>(campaignManager);
        logic->init();
//         client.stop();
        return true;
    });
    client.RegisterNetworkSystem(scriptEngine);
    client.initProcess();
    client.sendJoinRequest();
    while(!campaignManager.get()) {
        client.poll();
    }
    if(campaignManager.get()) {
        nDebug << "Join worked, yay! :)" << std::endl;
        while(1) {
            logic->step(0.1);
            client.poll();
            usleep(1000);
        }
    } else {
        nDebug << "Join request failed :(" << std::endl;
    }
    delete Ogre::ResourceGroupManager::getSingleton()._getResourceManager("ScriptFile");
    return 0;
}
