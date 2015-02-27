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

#ifndef NETWORK_RPCPACKAGE_H
#define NETWORK_RPCPACKAGE_H

#include "network.h"
#include "debug/ndebug.h"
#include <array>
#include <cstring>
#include <exception>
#include <memory>

class asIScriptEngine;

namespace network {

class RPCPackage
{
public:
    class overflow_exception : public std::exception
    {
    };

    static int RegisterType(std::shared_ptr<asIScriptEngine> engine);
    static std::shared_ptr<RPCPackage> make(rpc_id_t id);
    static std::shared_ptr<RPCPackage> make(rpc_id_t id, rpc_data_t data, std::string argString);

    void reset();
    void clear();

    rpc_data_t data() const { return m_data; }
    rpc_id_t rpcId() const { return m_rpcId; }
    void setRpcId(rpc_id_t id) { m_rpcId = id; }

    bool push(uint8_t val) { return pushGeneric(val, 'c'); }
    bool push(int8_t val) { return pushGeneric(val, 'C'); }
    bool push(uint16_t val) { return pushGeneric(val, 's'); }
    bool push(int16_t val) { return pushGeneric(val, 'S'); }
    bool push(uint32_t val) { return pushGeneric(val, 'i'); }
    bool push(int32_t val) { return pushGeneric(val, 'I'); }
    bool push(float val) { return pushGeneric(val, 'f'); }
    bool push(double val) { return pushGeneric(val, 'd'); }
    bool push(const std::string val);

    template<class T>
    T popValue()
    {
        if(m_readOffset + sizeof(T) > MAX_PACKAGE_SIZE) {
            throw overflow_exception();
        }
        T val = *reinterpret_cast<const T*>(&m_data[m_readOffset]);
        m_readOffset += sizeof(T);
        return val;
    }
    uint8_t popUint8() { return popValue<uint8_t>(); }
    int8_t popInt8() { return popValue<int8_t>(); }
    uint16_t popUint16() { return popValue<uint16_t>(); }
    int16_t popInt16() { return popValue<int16_t>(); }
    uint32_t popUint32() { return popValue<uint32_t>(); }
    int32_t popInt32() { return popValue<int32_t>(); }
    float popFloat() { return popValue<float>(); }
    double popDouble() { return popValue<double>(); }
    std::string popString();

    void AddRef();
    void Release();

    bool isValidPackage() const { return m_validPackage; }

    std::string argString() const { return m_argString; }
    bool isSizeCached() const { return m_isSizeCached; }
    size_t dataSize() const;

    size_t calculateDataSize() const;

private:
    static RPCPackage* make_getref(rpc_id_t id);
    RPCPackage(rpc_id_t id);
    RPCPackage(rpc_id_t id, rpc_data_t data, std::string argString);
    RPCPackage(const RPCPackage& other);
    ~RPCPackage() {}

    void cachePackageSize();

    template<class T>
    bool pushGeneric(const T& val, char argChar)
    {
        if(!m_isSizeCached) {
            cachePackageSize();
        }
        size_t newSize = m_packageSize + sizeof(val);
        if(newSize > MAX_RPC_DATA_SIZE) {
            m_validPackage = false;
            return false;
        }
        std::memcpy(m_data.data() + m_packageSize,
                    &val, sizeof(val)
        );
        m_packageSize = newSize;
        if(argChar != '\0') m_argString += argChar;
        return true;
    }

    rpc_data_t m_data;
    rpc_id_t m_rpcId;
    size_t m_readOffset = 0;
    size_t m_packageSize = 0;
    bool m_validPackage = true;
    bool m_isSizeCached = false;
    int m_refCount = 1;
    std::string m_argString;
};
}

#endif // NETWORK_RPCPACKAGE_H
