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

#include "rpcdispatcher.h"
#include "script/scriptengine.h"
#include "script/scriptfilemanager.h"
#include "network/network.pb.h"
#include "debug/ndebug.h"
#include <angelscript.h>
#include <scripthandle/scripthandle.h>
#include <cstring>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <list>

using namespace network;

RPCDispatcher::RPCDispatcher()
{

}

RPCDispatcher::~RPCDispatcher()
{

}

void RPCDispatcher::RegisterDispatcher(std::shared_ptr<script::ScriptEngine> engine)
{
    assert(!m_engine.get());
    m_engine = engine;
    assert(m_engine->engine().get());
    int r;
    r = m_engine->engine()->RegisterObjectType("RPC", 0, asOBJ_REF | asOBJ_NOHANDLE);
    assert(r >= 0);
//     r = m_engine->engine()->RegisterObjectBehaviour("RPC", asBEHAVE_FACTORY, "RPC@ f()",
//                                                     asMETHOD(RPCDispatcher,getThis), asCALL_THISCALL_ASGLOBAL);
//     assert(r >= 0);
    m_rpcObjectType = script::shared(m_engine->engine()->GetObjectTypeByName("RPC"));
    assert(m_rpcObjectType.get() != nullptr);
    r = m_engine->engine()->RegisterGlobalProperty("RPC rpc", this);
    assert(r >= 0);
    r = m_engine->engine()->RegisterGlobalFunction("bool __rpcRegister(uint16,string,string)",
                                                 asMETHOD(RPCDispatcher, registerRpc), asCALL_THISCALL_ASGLOBAL, this);
    assert(r >= 0);
    r = m_engine->engine()->RegisterGlobalFunction("uint32 __rpcRegisterHandler(uint16,uint16,string,string)",
                                                 asMETHODPR(RPCDispatcher,
                                                            addRpcHandler,
                                                            (uint16_t client_id, rpc_id_t id, std::string funcname, std::string argspec),
                                                            handler_id_t
                                                           ),
                                                   asCALL_THISCALL_ASGLOBAL, this);
    assert(r >= 0);
    r = m_engine->engine()->RegisterGlobalFunction("uint32 __rpcRegisterHandler(uint16,uint16,ref,string,string)",
                                                 asMETHODPR(RPCDispatcher,
                                                            addRpcHandler,
                                                            (uint16_t client_id, rpc_id_t id, CScriptHandle* handle, std::string funcname, std::string argspec),
                                                            handler_id_t
                                                           ),
                                                   asCALL_THISCALL_ASGLOBAL, this);
    assert(r >= 0);
    auto resource = script::ScriptFileManager::getSingleton().load(Ogre::String("rpc.tab"),
                                                                   Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    std::istringstream rpcConfig(resource->code());
    while(!rpcConfig.eof()) {
        procedure_t rpc;
        std::string function;
        rpcConfig >> rpc.id >> function >> rpc.argSpec;
        while(!rpcConfig.eof() && rpcConfig.get() != '\n');
        registerRpc(rpc.id, function, rpc.argSpec);
    }
}

bool RPCDispatcher::executeReceivedCall(const uint16_t& sender_client_id, std::shared_ptr<RPCPackage> package)
{
    assert(m_engine.get());
    handler_t handler;
    if(!findRpcHandler(handler, sender_client_id, package->rpcId())) {
        logWarning() << "Did not find handler for RPC ID " << package->rpcId() << " from caller ID " << sender_client_id << std::endl;
        return false;
    }
    if(handler.function) {
        auto ctx = m_engine->context();
        ctx->Prepare(handler.function);
        package->reset();
        if(handler.object) {
            ctx->SetObject(handler.object);
        }
        ctx->SetArgWord(0, sender_client_id);
        uint16_t argPos = 1;
        std::list<std::string> stringCache;
        for(char argType: handler.argSpec) {
            if(argType == 'c') ctx->SetArgByte(argPos, package->popValue<uint8_t>());
            else if(argType == 'C') ctx->SetArgByte(argPos, package->popValue<int8_t>());
            else if(argType == 's') ctx->SetArgWord(argPos, package->popValue<uint16_t>());
            else if(argType == 'S') ctx->SetArgWord(argPos, package->popValue<int16_t>());
            else if(argType == 'i') ctx->SetArgDWord(argPos, package->popValue<uint32_t>());
            else if(argType == 'I') ctx->SetArgDWord(argPos, package->popValue<int32_t>());
            else if(argType == 'd') ctx->SetArgDouble(argPos, package->popValue<double>());
            else if(argType == 'f') ctx->SetArgFloat(argPos, package->popValue<float>());
            else if(argType == 'L') {
                auto str = package->popString();
                stringCache.push_front(str);
                ctx->SetArgObject(argPos, &*stringCache.begin());
            }
            else assert(false);
            argPos++;
        }
        ctx->Execute();
    } else {
        handler.cppFunction(sender_client_id, package);
    }
    return true;
}

bool RPCDispatcher::executeReceivedCall(const uint16_t& sender_client_id, const rpc_id_t& id, const std::string& data)
{
    handler_t handler;
    if(!findRpcHandler(handler, sender_client_id, id)) {
        logWarning() << "Did not find handler for RPC ID " << id << " from caller ID " << sender_client_id << std::endl;
        return false;
    }
    rpc_data_t arrayData;
    std::memcpy(arrayData.data(), data.c_str(), std::min(data.length(), MAX_RPC_DATA_SIZE));
    auto package = RPCPackage::make(id, arrayData, handler.argSpec);
    return executeReceivedCall(sender_client_id, package);
}

bool RPCDispatcher::addRpcHandler(uint16_t client_id, rpc_id_t id, RPCDispatcher::callback_t function, const std::string& argspec)
{
    assert(m_engine.get());
    if(id > MAX_INTERNAL_RPC_ID) {
        logError() << "Tried to bind C++ function to ID " << id
                   << ", which is NOT in the reserved ID range! Ignoring handler..."
                   << std::endl;
        return false;
    }
    m_handlers[id] = handler_t { id, 0, 0, function, argspec, client_id };
    return true;
}

RPCDispatcher::handler_id_t RPCDispatcher::addRpcHandler(uint16_t client_id, rpc_id_t id, std::string funcname, std::string argspec)
{
    assert(m_engine.get());
    nDebug << "addRpcHandler " << funcname << "[" << argspec
           << "] to RPC ID " << id << std::endl;
    if(id <= MAX_INTERNAL_RPC_ID) {
        logError() << "Tried to bind AS function " << funcname
                   << " to ID " << id
                   << ", which is in the reserved ID range! Ignoring handler..."
                   << std::endl;
        return 0;
    }
    auto function = m_engine->engine()->GetModuleByIndex(0)->GetFunctionByName(funcname.c_str());
    if(!function) {
        logError() << "Failed to register RPC Handler ID " << id
                  << ", AS function name '" << funcname
                  << "', argspec '" << argspec << "'" << std::endl;
        return 0;
    }
    m_handlers[m_nextHandlerId] = handler_t { id, function, 0, 0, argspec, client_id };
    return m_nextHandlerId++;
}

RPCDispatcher::handler_id_t RPCDispatcher::addRpcHandler(uint16_t client_id, rpc_id_t id, CScriptHandle* handle, std::string funcname, std::string argspec)
{
    assert(m_engine.get());
    asIScriptObject* object = nullptr;
    handle->Cast((void**)&object, handle->GetTypeId());
    if(!object) {
        logError() << "No valid script object handle passed to __registerRpcHandler("
                   << client_id << ", "
                   << id << ", handle, "
                   << funcname << ", "
                   << argspec << ")!" << std::endl;
        return 0;
    }
    nDebug << "addRpcHandler " << object->GetObjectType()->GetName() << "::" << funcname
           << "[" << argspec << "] to RPC ID " << id << std::endl;
    if(id <= MAX_INTERNAL_RPC_ID) {
        logError() << "Tried to bind AS function " << object->GetObjectType()->GetName() << "::" << funcname
                   << " to ID " << id
                   << ", which is in the reserved ID range! Ignoring handler..."
                   << std::endl;
        return 0;
    }
    auto function = object->GetObjectType()->GetMethodByName(funcname.c_str());
    if(!function) {
        logError() << "Failed to register RPC Handler ID " << id
                  << ", AS function name '" << object->GetObjectType()->GetName()
                  << "::" << funcname << "', argspec '" << argspec << "'" << std::endl;
        return 0;
    }
    m_handlers[m_nextHandlerId] = handler_t { id, function, object, 0, argspec, client_id };
    return m_nextHandlerId++;
}

bool RPCDispatcher::registerRpc(rpc_id_t id, std::string name, std::string argspec)
{
    assert(m_engine.get());
    nDebug << "Register RPC " << id << " named '" << name << "', argspec '" << argspec << "'" << std::endl;
    if(id <= MAX_INTERNAL_RPC_ID) {
        logError() << "Tried to register RPC with ID " << id
                   << " named " << name
                   << ", which is in the reserved ID range! Won't add RPC..."
                   << std::endl;
        return false;
    }
    auto it = m_procedures.find(name);
    if(it != m_procedures.end()) {
        if(it->second.id == id && it->second.argSpec == argspec) {
            logInfo() << "Re-registered RPC " << id << " " << name << "[" << argspec << "]. Ignoring." << std::endl;
            return true;
        }
        logError() << "Cannot register RPC with name " << name
                   << ". RPC if same name is already registered to ID "
                   << it->second.id << "." << std::endl;
        return false;
    }
    m_procedures[name] = procedure_t { id, argspec };
    std::ostringstream decl;
    decl << "bool " << name << "(uint16"; // receiver_client_id
    for(char c: argspec) {
        decl << ", ";
        if(c == 'c') decl << "uint8";
        else if(c == 'C') decl << "int8";
        else if(c == 's') decl << "uint16";
        else if(c == 'S') decl << "int16";
        else if(c == 'i') decl << "uint32";
        else if(c == 'I') decl << "int32";
        else if(c == 'f') decl << "float";
        else if(c == 'd') decl << "double";
        else if(c == 'L') decl << "string";
        else assert(false);
    }
    decl << ")";
    nDebug << "Declaration for RPC wrapper object method: " << decl.str() << std::endl;
    auto r = m_engine->engine()->RegisterObjectMethod("RPC", decl.str().c_str(), asFUNCTION(RPCDispatcher::buildCall), asCALL_GENERIC);
    assert(r >= 0);
    m_rpcObjectType->GetMethodByName(name.c_str())->SetUserData(this);
    return true;
}

void RPCDispatcher::buildCall(asIScriptGeneric* gen)
{
    auto func = gen->GetFunction();
    auto dispatcher = reinterpret_cast<RPCDispatcher*>(func->GetUserData());
    nDebug << "Build RPC " << func->GetName() << std::endl;
    auto handler = dispatcher->m_procedures[func->GetName()];
    auto package = RPCPackage::make(handler.id);
    auto argnum = 1;
    uint16_t client_id = gen->GetArgWord(0);
    for(char c: handler.argSpec) {
        if(c == 'c') package->push((uint8_t)gen->GetArgByte(argnum));
        else if(c == 'C') package->push((int8_t)gen->GetArgByte(argnum));
        else if(c == 's') package->push((uint16_t)gen->GetArgWord(argnum));
        else if(c == 'S') package->push((int16_t)gen->GetArgWord(argnum));
        else if(c == 'i') package->push((uint32_t)gen->GetArgDWord(argnum));
        else if(c == 'I') package->push((int32_t)gen->GetArgDWord(argnum));
        else if(c == 'f') package->push(gen->GetArgFloat(argnum));
        else if(c == 'd') package->push(gen->GetArgDouble(argnum));
        else if(c == 'L') package->push(*reinterpret_cast<std::string*>(gen->GetAddressOfArg(argnum)));
        else assert(false);
        argnum++;
    }
    assert(handler.argSpec == package->argString());
    assert(dispatcher->m_sendRpcFunction);
    gen->SetReturnByte(dispatcher->m_sendRpcFunction(client_id, package));
}

bool RPCDispatcher::findRpcHandler(RPCDispatcher::handler_t& handler, const uint16_t& sender_id, const rpc_id_t& rpc_id) const
{
    for(const auto& kv: m_handlers) {
        const handler_t& possible_handler = kv.second;
        if(possible_handler.id != rpc_id) {
            continue;
        }
        if(possible_handler.client_id == 0 || possible_handler.client_id == sender_id) {
            handler = possible_handler;
            return true;
        }
    }
    return false;
}

