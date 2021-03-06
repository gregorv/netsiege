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
#include <script/scriptengine.h>
#include <angelscript.h>

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <signal.h>

// using boost::asio;
// using boost::posix_time;

// boost::bind hates me!
network::NetworkServer* g_daServer = nullptr;
void handle_signal(const boost::system::error_code& error, int signal_number)
{
    if(g_daServer)
        g_daServer->stopServer();
}


namespace network {

int NetworkServer::RegisterNetworkSystem(std::shared_ptr< script::ScriptEngine > engine)
{
    RPCPackage::RegisterType(engine->engine());
    RegisterDispatcher(engine);
    addRpcHandler(0, RPC_ID_JOIN_SERVER_REQ,
                  std::bind(&NetworkServer::handle_joinRequest, this,
                            std::placeholders::_1, std::placeholders::_2
                ),
                  RPC_ARGS_JOIN_SERVER_REQ
    );
    addRpcHandler(0, RPC_ID_JOIN_SERVER_ACK,
                  std::bind(&NetworkServer::handle_joinAcknowledge, this,
                            std::placeholders::_1, std::placeholders::_2
                ),
                  RPC_ARGS_JOIN_SERVER_ACK
    );
    setRpcSendFunction(std::bind(&NetworkServer::remoteProcedureCall, this,
                                 std::placeholders::_1, std::placeholders::_2));
    return 0;
}

NetworkServer::NetworkServer(const udp::endpoint& interface)
: m_ioservice(), m_syncTimer(m_ioservice, boost::posix_time::milliseconds(SYNC_PERIOD*1000.0)),
  m_callbackTimer(m_ioservice, boost::posix_time::milliseconds(2000)),
  m_signals(m_ioservice, SIGINT, SIGTERM),
  m_socket(m_ioservice, interface)
{
    g_daServer = this;
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
    m_signals.async_wait(handle_signal);
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
    m_callbackTimer.expires_from_now(boost::posix_time::milliseconds(m_callbackTimerTimeout*1000.0));
    m_callbackTimer.async_wait(boost::bind(&NetworkServer::handle_timeoutCallback, this));
}



void NetworkServer::send(const udp::endpoint& remoteEndpoint, const package_buffer_t& package, size_t nBytes)
{
    nDebugL(4) << "NetworkServer::send(" << remoteEndpoint << ", [package], " << nBytes << ")" << std::endl;
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

bool NetworkServer::remoteProcedureCall(uint16_t client_id, std::shared_ptr< network::RPCPackage > package)
{
    bool r = true;
    for(auto pair: m_clients) {
        std::shared_ptr<ClientSession> client = pair.second;
        if(!client->isActive()) {
            continue;
        }
        if(client_id > 0 && client->clientId() != client_id) {
            continue;
        }
        if(!client->remoteProcedureCall(package)) {
            r = false;
        }
    }
    return r;
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
        for(auto id: m_objectManager->removedIds()) {
            msg->mutable_world_state_update()->add_removed_ids(id);
        }
        for(auto client: m_clients) {
            if(client.second->isActive()) {
                client.second->sendPackage(msg);
            }
        }
    }
    m_objectManager->flushUpdateCache();
    m_syncTimer.expires_from_now(boost::posix_time::milliseconds(SYNC_PERIOD*1000.0));
    m_syncTimer.async_wait(boost::bind(&NetworkServer::syncTimeout, this));
    closeDeadConnections();
}

void NetworkServer::handle_receive(const boost::system::error_code& error,
                                   std::size_t bytesTransferred)
{
    nDebugL(3) << "Received " << bytesTransferred << " bytes from " << m_remoteEndpoint << std::endl;
    auto it = m_clients.find(m_remoteEndpoint);
    if(it == m_clients.end()) {
        logInfo() << "Client connected from " << m_remoteEndpoint << std::endl;
        m_clients[m_remoteEndpoint] = std::make_shared<ClientSession>(m_nextClientId++, m_remoteEndpoint, this);
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
    m_callbackTimer.expires_from_now(boost::posix_time::milliseconds(m_callbackTimerTimeout*1000.0));
    m_callbackTimer.async_wait(boost::bind(&NetworkServer::handle_timeoutCallback, this));
}

void NetworkServer::handle_joinRequest(uint16_t client_id, std::shared_ptr<RPCPackage> package)
{
    auto clientVersion = package->popValue<uint32_t>();
    auto clientName = package->popString();
    nDebug << "handle_joinRequest: Client ID " << client_id << ", Client Version " << clientVersion << ", client name " << clientName << std::endl;
    auto response = RPCPackage::make(RPC_ID_JOIN_SERVER_RESP);
//     uint32 player_id, uint32 server_version, char player_accepted, string map_name, uint32 map_version
    response->push((uint32_t)client_id);
    response->push((uint32_t)0); //d server version
    response->push((uint8_t)true);
    response->push("test");
    response->push((uint32_t)0);
    for(auto& kv: m_clients) {
        if(kv.second->clientId() == client_id) {
            kv.second->isActive(true);
        }
    }
    remoteProcedureCall(client_id, response);
    assert(response->argString() == RPC_ARGS_JOIN_SERVER_RESP);

}

void NetworkServer::handle_joinAcknowledge(uint16_t client_id, std::shared_ptr<RPCPackage> package)
{
    bool accepted = static_cast<bool>(package->popValue<uint8_t>());
    nDebug << "handle_joinAcknowledge: Client ID " << client_id << ", Accepted " << accepted << std::endl;
    std::shared_ptr<ClientSession> client;
    for(auto& kv: m_clients) {
        if(kv.second->clientId() == client_id) {
            client = kv.second;
        }
    }
    if(accepted) {
        auto msg = std::make_shared<pb::S2CMessage>();
        m_objectManager->serializeFull(msg->mutable_world_state_update()->mutable_updated_objects());
        client->isActive(true);
        client->sendPackage(msg);
        nDebug << "Join request of client " << client_id << " acknowledged!" << std::endl;
    } else {
        nDebug << "Join of client " << client_id << " refused by client." << std::endl;
        m_clients.erase(client->getEndpoint());
    }

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

