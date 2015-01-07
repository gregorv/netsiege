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
#include "script/scriptengine.h"
#include "angelscript.h"
#include <boost/bind.hpp>

namespace network {

NetworkClient::NetworkClient(const udp::endpoint& serverEndpoint, const std::string& playerName)
: UdpConnection(serverEndpoint, this, this), m_name(playerName),
m_socket(m_ioservice, udp::endpoint(udp::v4(), 0))
{

}

NetworkClient::~NetworkClient()
{

}

int NetworkClient::RegisterNetworkSystem(std::shared_ptr< script::ScriptEngine > engine)
{
    RPCPackage::RegisterType(engine->engine());
    RegisterDispatcher(engine);
    addRpcHandler(0, RPC_ID_JOIN_SERVER_RESP,
                  std::bind(&NetworkClient::handleJoinResponse, this,
                            std::placeholders::_1, std::placeholders::_2
                ),
                  RPC_ARGS_JOIN_SERVER_RESP
    );
    setRpcSendFunction(std::bind(&NetworkClient::remoteProcedureCall, this,
                                 std::placeholders::_1, std::placeholders::_2));
    return 0;
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
        boost::bind(&NetworkClient::handle_send, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred(),
            package.size()
            )
    );
}

bool NetworkClient::remoteProcedureCall(uint16_t receiverClientId, std::shared_ptr< RPCPackage > package)
{
    return UdpConnection::remoteProcedureCall(package);
}

void NetworkClient::initProcess()
{
    addRpcHandler(0,
                  RPC_ID_JOIN_SERVER_RESP,
                  std::bind(&NetworkClient::handleJoinResponse, this,
                            std::placeholders::_1, std::placeholders::_2),
                  RPC_ARGS_JOIN_SERVER_RESP
    );
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

void NetworkClient::sendJoinRequest()
{
    auto package = RPCPackage::make(RPC_ID_JOIN_SERVER_REQ);
    package->push((uint32_t)0);
    package->push(m_name);
    assert(package->argString() == RPC_ARGS_JOIN_SERVER_REQ);
    remoteProcedureCall(0, package);
}

void NetworkClient::setJoinAcceptHandler(join_accept_callback_t callback)
{
    m_acceptCallback = callback;
}

void NetworkClient::handle_receive(const boost::system::error_code& error,
                                   std::size_t bytesTransferred)
{
    nDebugL(4) << "NetworkClient::handle_receive(" << error << ", " << bytesTransferred << ")" << std::endl;
    pb::S2CMessage msg;
    msg.ParseFromArray(&m_receiveBuffer.front(),
                       m_receiveBuffer.size());
    if(!parsePackage(msg)) {
        // TODO: apply changes to scene manager
        // TODO Server negotiation
    }
    listen();
}

void NetworkClient::handle_send(const boost::system::error_code& error, std::size_t bytesTransferred, std::size_t bytesExcpected)
{

}

void NetworkClient::handleJoinResponse(uint16_t client_id, std::shared_ptr<RPCPackage> package)
{
    assert(m_acceptCallback);
    nDebug << "handleJoinResponse" << std::endl;
    uint32_t player_id = package->popValue<uint32_t>();
    uint32_t server_version = package->popValue<uint32_t>();
    char player_accepted = package->popValue<char>();
    std::string map_name = package->popString();
    uint32_t map_version = package->popValue<uint32_t>();
    nDebugVerbose << "Response arguments: " << "player_id " << player_id << ", "
                                            << "server_version " << server_version << ", "
                                            << "player_accepted " << (player_accepted? std::string("true"):std::string("false")) << ", "
                                            << "map_name " << map_name << ", "
                                            << "map_version " << map_version << std::endl;
    bool loadSuccessfull = m_acceptCallback(player_id, server_version, player_accepted, map_name, map_version);
    auto rpc = RPCPackage::make(RPC_ID_JOIN_SERVER_ACK);
    rpc->push(static_cast<uint8_t>(loadSuccessfull));
    assert(rpc->argString() == RPC_ARGS_JOIN_SERVER_ACK);
    remoteProcedureCall(0, rpc);
}

}
