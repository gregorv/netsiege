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

#include "rpcpackage.h"

#include <cassert>
#include "angelscript.h"
#include "script/scriptengine.h"

using namespace network;


int RPCPackage::RegisterType(std::shared_ptr< asIScriptEngine > engine)
{
    auto r = engine->RegisterObjectType("RPCPackage", 0, asOBJ_REF);
    assert(r >= 0);
    r = engine->RegisterObjectBehaviour("RPCPackage", asBEHAVE_ADDREF, "void f()", asMETHOD(RPCPackage,AddRef), asCALL_THISCALL);
    assert(r >= 0);
    r = engine->RegisterObjectBehaviour("RPCPackage", asBEHAVE_RELEASE, "void f()", asMETHOD(RPCPackage,Release), asCALL_THISCALL);
    assert(r >= 0);
    r = engine->RegisterObjectBehaviour("RPCPackage", asBEHAVE_FACTORY, "RPCPackage@ f(uint16 id)", asFUNCTION(RPCPackage::make_getref), asCALL_CDECL);
    assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "bool reset()", asMETHOD(RPCPackage, reset), asCALL_THISCALL);
    assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "bool push(int)", asMETHODPR(RPCPackage, push, (int32_t), bool), asCALL_THISCALL);
    assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "bool push(uint)", asMETHODPR(RPCPackage, push, (uint32_t), bool), asCALL_THISCALL);
    assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "bool push(int16)", asMETHODPR(RPCPackage, push, (int16_t), bool), asCALL_THISCALL);
    assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "bool push(uint16)", asMETHODPR(RPCPackage, push, (uint16_t), bool), asCALL_THISCALL);
    assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "bool push(int8)", asMETHODPR(RPCPackage, push, (int8_t), bool), asCALL_THISCALL);
    assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "bool push(uint8)", asMETHODPR(RPCPackage, push, (uint8_t), bool), asCALL_THISCALL);
    assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "bool push(float)", asMETHODPR(RPCPackage, push, (float), bool), asCALL_THISCALL);
    assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "bool push(double)", asMETHODPR(RPCPackage, push, (double), bool), asCALL_THISCALL);
    assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "bool push(const string)", asMETHODPR(RPCPackage, push, (const std::string), bool), asCALL_THISCALL);
    assert(r >= 0);

    r = engine->RegisterObjectMethod("RPCPackage", "uint8 uint8Pop()", asMETHOD(RPCPackage, popUint8), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "int8 int8Pop()", asMETHOD(RPCPackage, popInt8), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "uint16 uint16Pop()", asMETHOD(RPCPackage, popUint16), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "int16 int16Pop()", asMETHOD(RPCPackage, popInt16), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "uint32 uint32Pop()", asMETHOD(RPCPackage, popUint32), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "int32 int32Pop()", asMETHOD(RPCPackage, popInt32), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "float floatPop()", asMETHOD(RPCPackage, popFloat), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "double doublePop()", asMETHOD(RPCPackage, popDouble), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "string strPop()", asMETHOD(RPCPackage, popString), asCALL_THISCALL); assert(r >= 0);

    r = engine->RegisterObjectMethod("RPCPackage", "uint16 rpcId()", asMETHOD(RPCPackage, rpcId), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "void SetRpcId(uint16)", asMETHOD(RPCPackage, setRpcId), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "string argString()", asMETHOD(RPCPackage, argString), asCALL_THISCALL); assert(r >= 0);
    r = engine->RegisterObjectMethod("RPCPackage", "bool isValidPackage()", asMETHOD(RPCPackage, isValidPackage), asCALL_THISCALL); assert(r >= 0);
    return 0;
}

std::shared_ptr< RPCPackage > RPCPackage::make(rpc_id_t id)
{
    return script::shared(new RPCPackage(id));
}

std::shared_ptr< RPCPackage > RPCPackage::make(rpc_id_t id, rpc_data_t data, std::string argString)
{
    return script::shared(new RPCPackage(id, data, argString));
}

RPCPackage* RPCPackage::make_getref(rpc_id_t id)
{
    return new RPCPackage(id);
}

RPCPackage::RPCPackage(rpc_id_t id)
 : m_data(), m_rpcId(id)
{
}

RPCPackage::RPCPackage(rpc_id_t id, rpc_data_t data, std::string argString)
 : m_data(data), m_rpcId(id), m_argString(argString)
{
}

RPCPackage::RPCPackage(const RPCPackage& other)
 : m_data(other.m_data), m_rpcId(other.m_rpcId), m_argString(other.m_argString)
{
}

size_t RPCPackage::calculateDataSize() const
{
    size_t size = 0;
    for(auto c: m_argString) {
        if(c == 'c' || c == 'C') size += 1;
        else if(c == 's' || c == 'S') size += 2;
        else if(c == 'i' || c == 'I' || c =='f') size += 4;
        else if(c == 'd') size += 8;
        else if(c == 'L') {
            // read the length of the string and add this to the offset
            size += *reinterpret_cast<const uint16_t*>(m_data.data() + size);
        }
        else assert(false);
    }
    return size;
}

void RPCPackage::cachePackageSize()
{
    m_isSizeCached = true;
    m_packageSize = calculateDataSize();
}

void RPCPackage::reset()
{
    m_readOffset = 0;
}

void RPCPackage::clear()
{
    m_isSizeCached = true;
    m_packageSize = 0;
    m_validPackage = true;
    m_argString.clear();
}

size_t RPCPackage::dataSize() const
{
    if(!m_isSizeCached) {
        calculateDataSize();
    }
    return m_packageSize;
}

bool RPCPackage::push(const std::string val)
{
    if(!m_isSizeCached) {
        cachePackageSize();
    }
    size_t newSize = m_packageSize + val.length() + 2;
    if(newSize > MAX_RPC_DATA_SIZE) {
        m_validPackage = false;
        return false;
    }
    auto r = pushGeneric<uint16_t>(val.length(), '\0');
    assert(r == true);
    assert(m_packageSize + val.length() == newSize);
    std::memcpy(m_data.data() + m_packageSize,
                val.c_str(),
                val.length());
    m_packageSize = newSize;
    m_argString += 'L';
    return true;
}

std::string RPCPackage::popString()
{
    uint16_t strlen = popValue<uint16_t>();
    if(m_readOffset + strlen > MAX_RPC_DATA_SIZE) {
        logWarning() << "Malformed RPC received. String tries to read beyond max package"
                        "size. Possible hacking attempt? Clamping string to maximum safe size."
                        << std::endl;
        strlen = MAX_RPC_DATA_SIZE - m_readOffset;
    }
    return std::string(&m_data[m_readOffset], strlen);
}


void RPCPackage::AddRef()
{
    m_refCount++;
}

void RPCPackage::Release()
{
    if(--m_refCount <= 0) {
        delete this;
    }
}
