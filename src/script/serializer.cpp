/*
 * netsiege - A multiplayer point and click adventure
 * Copyright (C) 2014-2015 Gregor Vollmer <gregor@celement.de>
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

#include "serializer.h"
#include "angelscript.h"
#include "scriptengine.h"
#include "debug/ndebug.h"
#include "scriptarray/scriptarray.h"
#include <script/script.pb.h>
#include <cassert>
#include <string.h>
#include <algorithm>
#include <boost/algorithm/string.hpp>

using namespace script;

bool decodePrimitive(const pb::Property& src, void* propAddress, size_t index)
{
    if(index >= src.primitve_dat_size()) {
        logError() << "Decoding primitive property, index is out of src range!" << std::endl;
        return false;
    }
    assert(propAddress != nullptr);
    auto size = src.primitive_size();
    uint64_t data = src.primitve_dat(index);
    if(size > sizeof(data)) {
        logError() << "Decoding primitive: primitive size is invalid, "
                   << size << " > sizeof(uint64_t)" << std::endl;
        return false;
    }
    memcpy(propAddress, &data, size);
    return true;
}

bool decodeString(const pb::Property& src, void* propAddress, size_t index)
{
    if(index >= src.string_dat_size()) {
        logError() << "Decoding string property, index is out of src range!" << std::endl;
        return false;
    }
    assert(propAddress != nullptr);
    *reinterpret_cast<std::string*>(propAddress) = src.string_dat(index);;
    return true;
}

bool deserializeSingular(const pb::Property& src, asIScriptObject* obj, size_t propertyIndex, const std::string& decl, uint32_t typeId)
{
    assert(propertyIndex <obj->GetPropertyCount());
    if(typeId > asTYPEID_VOID && typeId <= asTYPEID_DOUBLE) {
        return decodePrimitive(src, obj->GetAddressOfProperty(propertyIndex), 0);
    } else if(decl == "string") {
        return decodeString(src, obj->GetAddressOfProperty(propertyIndex), 0);
    }
    return false;
}

bool deserializeArray(const pb::Property& src, asIScriptObject* obj, size_t propertyIndex, const std::string& decl, uint32_t typeId)
{
    assert(propertyIndex < obj->GetPropertyCount());
    CScriptArray* array = reinterpret_cast<CScriptArray*>(obj->GetAddressOfProperty(propertyIndex));
    while(array->GetSize()) array->RemoveAt(0);
    if(typeId > asTYPEID_VOID && typeId <= asTYPEID_DOUBLE) {
        for(size_t i=0; i < src.primitve_dat_size(); i++) {
            uint64_t dat;
            if(!decodePrimitive(src, &dat, i)) {
                return false;
            }
            array->InsertLast(&dat);
        }
    } else if(decl == "string") {
        for(size_t i=0; i < src.string_dat_size(); i++) {
            std::string dat;
            if(!decodeString(src, &dat, i)) {
                return false;
            }
            array->InsertLast(&dat);
        }
    }
    return true;
}


Serializer::Serializer(script::ScriptEngine* engine)
 : m_engine(engine)
{
}

void Serializer::serializeObject(script::pb::ScriptObject* target,
                                   std::shared_ptr< asIScriptObject > obj,
                                   const std::vector<std::string>& properties)
{
    serializeObject(target, obj.get(), properties);
}

void Serializer::serializeObject(pb::ScriptObject* target,
                                   asIScriptObject* obj,
                                   const std::vector< std::string >& properties)
{
    assert(obj);
    assert(target);
    for(size_t i=0; i < obj->GetPropertyCount(); i++) {
        std::string propName(obj->GetPropertyName(i));
        bool storeProperty = false;
        for(auto n: properties) {
            if(n == propName) {
                storeProperty = true;
                break;
            }
        }
        if(!storeProperty) continue;
        auto typeId = obj->GetPropertyTypeId(i);
        std::string decl(m_engine->engine()->GetTypeDeclaration(typeId));
        if(typeId > asTYPEID_VOID && typeId <= asTYPEID_DOUBLE) {
            auto property = target->add_properties();
            property->set_name(propName);
            property->set_decl(decl);
            auto size = m_engine->engine()->GetSizeOfPrimitiveType(typeId);
            uint64_t data = 0;
            assert(size < sizeof(data));
//             memcpy(&data + (sizeof(data) - size), obj->GetAddressOfProperty(i), size);
            memcpy(&data, obj->GetAddressOfProperty(i), size);
            property->set_primitive_size(size);
            property->add_primitve_dat(data);
        } else if(decl == "string") {
            auto property = target->add_properties();
            property->set_name(propName);
            property->set_decl(decl);
            std::string data{*reinterpret_cast<std::string*>(obj->GetAddressOfProperty(i))};
            property->add_string_dat(data);
        } else if(decl == "string[]") {
            auto array = reinterpret_cast<CScriptArray*>(obj->GetAddressOfProperty(i));
            auto property = target->add_properties();
            property->set_name(propName);
            property->set_decl(decl);
            for(size_t idx=0; idx<array->GetSize(); idx++) {
                assert(array->At(idx) != nullptr);
                std::string element(*reinterpret_cast<const std::string*>(array->At(idx)));
                property->add_string_dat(element);
            }
        } else if(decl[decl.length()-2]=='[' && decl[decl.length()-1]==']') {
            auto array = reinterpret_cast<CScriptArray*>(obj->GetAddressOfProperty(i));
            auto elementTypeId = array->GetElementTypeId();
            if(elementTypeId > asTYPEID_VOID && elementTypeId <= asTYPEID_DOUBLE) {
                auto property = target->add_properties();
                auto size = m_engine->engine()->GetSizeOfPrimitiveType(typeId);
                property->set_name(propName);
                property->set_decl(decl);
                property->set_primitive_size(size);
                uint64_t data = 0;
                assert(size < sizeof(data));
                for(size_t idx=0; idx<array->GetSize(); idx++) {
                    memcpy(&data, array->At(idx), size);
                    property->add_primitve_dat(data);
                }
            } else {
                logError() << "Cannot serialize property '" << propName
                    << "', arrays only supported for primary types and strings, not for "
                    << decl << std::endl;
            }
        } else if(typeId & asTYPEID_MASK_OBJECT) {
            logWarning() << "NOT IMPLEMENTED! Serializing nested objects not supported" << std::endl;
        } else {
            logError() << "Cannot serialize property '" << propName << "', type '" << decl << "' cannot be serialized!" << std::endl;
        }
    }
}

bool Serializer::deserializeObject(const pb::ScriptObject& source, asIScriptObject* obj)
{
    for(size_t i=0; i<source.properties_size(); i++) {
        size_t propIndex = 0;
        for(; propIndex < obj->GetPropertyCount(); propIndex++) {
            if(std::string(obj->GetPropertyName(propIndex)) == source.properties(i).name()) {
                break;
            }
        }
        if(!deserializeProperty(source.properties(i), obj, propIndex)) {
            return false;
        }
    }
    return true;
}

bool Serializer::deserializeProperty(const pb::Property& source, asIScriptObject* obj, size_t propertyIndex)
{
    std::string propName(obj->GetPropertyName(propertyIndex));
    std::string propDecl(m_engine->engine()->GetTypeDeclaration(obj->GetPropertyTypeId(propertyIndex)));
    assert(propName == source.name());
    if(propDecl != source.decl()) {
        logError() << "Cannot deserialize property " << propName
                   << " because the types don't match. Received type "
                   << source.decl() << ", script type " << propDecl << std::endl;
        return false;
    }
    auto typeId = obj->GetPropertyTypeId(propertyIndex);
    if(propDecl[propDecl.size()-2] == '[' && propDecl[propDecl.size()-1] == ']') {
        return deserializeArray(source, obj, propertyIndex, propDecl, typeId);
    } else {
        return deserializeSingular(source, obj, propertyIndex, propDecl, typeId);
    }
    return false;
}

bool Serializer::registerClass(const std::string& className, const std::string& properties)
{
    // TODO Implement!
    return true;
}

std::vector< std::string > Serializer::getPropertyList(const std::string& properties) const
{
    std::vector<std::string> props;
    boost::split(props, properties, [](char c){ return c == ','; });
    for(auto& p: props) {
        boost::trim(p);
    }
    return props;
}

bool Serializer::isSerializableProperty(uint32_t typeId) const
{
    std::string decl(m_engine->engine()->GetTypeDeclaration(typeId));
    if(typeId > asTYPEID_VOID && typeId <= asTYPEID_DOUBLE) {
        return true;
    } else if(decl == "string") {
        return true;
    } else if(decl == "string[]") {
       return true;
    } else if(decl[decl.length()-2]=='[' && decl[decl.length()-2]==']') {
        return true;
    } else if(typeId & asTYPEID_MASK_OBJECT) {
        logWarning() << "TODO  Check if object is realy serializable!" << std::endl;
        return true;
    }
    return false;
}

bool Serializer::isSerializableType(asIScriptObject* object, const std::vector< std::string >& properties) const
{
    for(size_t i=0; i<object->GetPropertyCount(); i++) {
        std::string name(object->GetPropertyName(i));
        if(std::find(properties.begin(), properties.end(), name) == properties.end()) {
            continue;
        }
        if(!isSerializableProperty(object->GetPropertyTypeId(i))) {
            return false;
        }
    }
    return true;
}
