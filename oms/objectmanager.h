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

namespace oms {

class ObjectManager
{
public:
    typedef std::set<std::shared_ptr<GameObject>> objectSet_t;
    typedef std::map<GameObject::id_t, std::shared_ptr<GameObject>> objectIdMap_t;
    typedef std::set<GameObject::id_t> idSet_t;

    ObjectManager();
    ~ObjectManager();

    const std::shared_ptr< GameObject >& createObject(const std::string& name);
    void removeObject(GameObject::id_t id);
    void removeObject(const std::shared_ptr<GameObject>& obj);

    void clear();

    std::shared_ptr<GameObject> findObject(const std::string& name) const;
    const std::shared_ptr<GameObject>& findObject(GameObject::id_t id) const;
    void addUpdatedObject(const std::shared_ptr<GameObject>& obj);
    void flushUpdateCache();

    void serializeFull(omsproto::GameObjectSet* object_set) const;
    void serializeChanges(omsproto::GameObjectSet* object_set) const;
    void deserialize(omsproto::GameObjectSet* object_set);

    const objectSet_t& getUpdatedObjects() const { return m_alteredObjects; }
    const objectSet_t& getNewObjects() const { return m_newObjects; }
    const idSet_t& getRemovedIds() const { return m_removedIds; }
    const objectIdMap_t& getObjects() const { return m_objectsById; }

private:
    objectSet_t m_alteredObjects;
    objectSet_t m_newObjects;
    idSet_t m_removedIds;
    objectIdMap_t m_objectsById;
};

}

#endif // OBJECTMANAGER_H
