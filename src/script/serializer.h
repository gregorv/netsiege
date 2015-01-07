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

#ifndef SCRIPT_SERIALIZER_H
#define SCRIPT_SERIALIZER_H

#include <memory>
#include <string>
#include <vector>

class asIScriptObject;

namespace script {

namespace pb {
    class ScriptObject;
    class Property;
}

class ScriptEngine;

class Serializer
{
public:
    Serializer(ScriptEngine* engine);

    bool isSerializableProperty(uint32_t typeId) const;
    bool isSerializableType(asIScriptObject* object, const std::vector<std::string>& properties) const;
    bool isSerializableType(std::shared_ptr<asIScriptObject> object, const std::vector<std::string>& properties) const
    {
        return isSerializableType(object.get(), properties);
    }
    std::vector<std::string> getPropertyList(const std::string& properties) const;

    void serializeObject(script::pb::ScriptObject* target, std::shared_ptr<asIScriptObject> obj, const std::vector<std::string>& properties);
    void serializeObject(script::pb::ScriptObject* target, asIScriptObject* obj, const std::vector<std::string>& properties);

    void deserializeObject(const script::pb::ScriptObject& source, std::shared_ptr<asIScriptObject> obj) { deserializeObject(source, obj.get()); }
    bool deserializeObject(const script::pb::ScriptObject& source, asIScriptObject* obj);

private:
    bool deserializeProperty(const script::pb::Property& source, asIScriptObject* obj, size_t propertyIndex);
    bool registerClass(const std::string& className, const std::string& properties);

    ScriptEngine* m_engine;
};
}

#endif // SCRIPT_SERIALIZER_H
