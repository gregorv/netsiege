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

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <map>
#include <set>
#include <string>
#include <memory>
#include <ios>

#include "gameobject.h"

namespace omsproto {
    class GameObjectSet;
}

namespace script {
    class ScriptEngine;
}

class asIScriptObject;

namespace oms {

class ObjectManager
{
public:
    typedef std::set<std::shared_ptr<GameObject>> objectSet_t;
    typedef std::map<GameObject::id_t, std::shared_ptr<GameObject>> objectIdMap_t;
    typedef std::set<GameObject::id_t> idSet_t;

    ObjectManager();
    ~ObjectManager();

    void setScriptEngine(std::shared_ptr<script::ScriptEngine> engine);
    void setModeClient(bool isClientManager) { m_modeClient = isClientManager; }

    std::shared_ptr< GameObject > createObject(const std::string& type, const std::string& name);
    asIScriptObject* createObjectRet(const std::string& type, const std::string& name);
    void removeObject(GameObject::id_t id);
    void removeObject(const std::shared_ptr<GameObject>& obj);

    void clear();

    std::shared_ptr<GameObject> findObject(const std::string& name) const;
    std::shared_ptr<GameObject> findObject(GameObject::id_t id) const;
    void addUpdatedObject(const std::shared_ptr<GameObject>& obj);
    void flushUpdateCache();

    void serializeFull(omsproto::GameObjectSet* object_set) const;
    void serializeChanges(omsproto::GameObjectSet* object_set) const;
    void deserialize(const omsproto::GameObjectSet* object_set);

    objectSet_t updatedObjects() const { return m_alteredObjects; }
    objectSet_t newObjects() const { return m_newObjects; }
    idSet_t removedIds() const { return m_removedIds; }
    objectIdMap_t objects() const { return m_objectsById; }

    void step(float dt);

private:
    std::shared_ptr< GameObject > createObject(const std::string& typeName, const std::string& name, bool autoId, GameObject::id_t id);
    asIScriptObject* asFindObject(const std::string& name);
    asIScriptObject* asFindObject(GameObject::id_t id);
    bool asObjectExists(const std::string& name);
    bool asObjectExists(GameObject::id_t id);

    bool registerObject(const std::string& name, const std::string& className, const std::string& syncProperties, uint32_t flags);
    bool registerObject(const std::string& name, const std::string& serverClass, const std::string& clientClass, const std::string& syncProperties, uint32_t flags);

    std::map<std::string,GameObject::scriptObjectInfo_t> m_registeredTypes;

    objectSet_t m_alteredObjects;
    objectSet_t m_newObjects;
    idSet_t m_removedIds;
    objectIdMap_t m_objectsById;
    std::shared_ptr<script::ScriptEngine> m_scriptEngine;
    bool m_modeClient;
};

}

#endif // OBJECTMANAGER_H
