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

#ifndef NETWORK_RPCHANDLER_H
#define NETWORK_RPCHANDLER_H

#include "network/network.h"
#include "rpcpackage.h"
#include <memory>
#include <string>
#include <exception>
#include <cstring>
#include <map>

class asIScriptFunction;
class asIScriptGeneric;
class asIScriptObject;
class asIObjectType;
class CScriptHandle;

namespace script {
    class ScriptEngine;
}

namespace network {

class RPCDispatcher
{
public:
    typedef std::function<void(uint16_t, std::shared_ptr<RPCPackage>)> callback_t;
    typedef std::function<bool(uint16_t, std::shared_ptr<RPCPackage>)> send_rpc_function_t;
    typedef uint32_t handler_id_t;
    struct handler_t {
        rpc_id_t id;
        asIScriptFunction* function;
        asIScriptObject* object;
        callback_t cppFunction;
        std::string argSpec;
        uint16_t client_id;
    };

    struct procedure_t {
        rpc_id_t id;
        std::string argSpec;
    };


    RPCDispatcher();
    virtual ~RPCDispatcher();

    void RegisterDispatcher(std::shared_ptr<script::ScriptEngine> engine);
    bool loadRpcHandlerSpec(const std::string& filename="rpc.tab");

    bool executeReceivedCall(const uint16_t& sender_client_id, std::shared_ptr< network::RPCPackage > package);
    bool executeReceivedCall(const uint16_t& sender_client_id, const rpc_id_t& id, const std::string& data);

    bool addRpcHandler(uint16_t client_id, rpc_id_t id, network::RPCDispatcher::callback_t function, const std::string& argspec);

    void setRpcSendFunction(send_rpc_function_t func) { m_sendRpcFunction = func; }

private:
    handler_id_t addRpcHandler(uint16_t client_id, rpc_id_t id, std::string funcname, std::string argspec);
    handler_id_t addRpcHandler(uint16_t client_id, rpc_id_t id, CScriptHandle* handle, std::string funcname, std::string argspec);
    bool registerRpc(rpc_id_t id, std::string name, std::string argspec);
    static void buildCall(asIScriptGeneric* gen);

    bool findRpcHandler(network::RPCDispatcher::handler_t& handler, const uint16_t& sender_id, const rpc_id_t& rpc_id) const;

    RPCDispatcher* getThis() { return this; }

private:
    /// Container for RPC Handlers. Key: Handler ID
    std::map<handler_id_t, handler_t> m_handlers;
    /// Container for known RPCs. Key: Name of RPC method
    std::map<std::string, procedure_t> m_procedures;
    std::shared_ptr<script::ScriptEngine> m_engine;
    std::shared_ptr<asIObjectType> m_rpcObjectType;
    handler_id_t m_nextHandlerId = 1;
    send_rpc_function_t m_sendRpcFunction;
};
}

#endif // NETWORK_RPCHANDLER_H
