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

#include "objectmanager.h"
#include "objects.pb.h"

#include <cassert>
#include <stdexcept>

namespace oms {

ObjectManager::ObjectManager()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

ObjectManager::~ObjectManager()
{
    clear();
}

std::shared_ptr< GameObject > ObjectManager::createObject(const std::string& name)
{
    auto newObject = std::make_shared<GameObject>(name);
    m_newObjects.insert(newObject);
    m_objectsById[newObject->id()] = newObject;
    return m_objectsById[newObject->id()];
}

void ObjectManager::removeObject(GameObject::id_t id)
{
    if(m_objectsById.erase(id) != 0) {
        m_removedIds.insert(id);
    }
}

void ObjectManager::removeObject(const std::shared_ptr< GameObject >& obj)
{
    if(obj.get()) {
        removeObject(obj->id());
    }
}

void ObjectManager::clear()
{
    m_alteredObjects.clear();
    m_newObjects.clear();
    m_objectsById.clear();
}

std::shared_ptr< GameObject > ObjectManager::findObject(const std::string& name) const
{
    for(auto it = m_objectsById.begin(); it != m_objectsById.end(); it++) {
        if(it->second->name() == name) {
            return it->second;
        }
    }
    throw std::out_of_range(std::string("Object with specified name not found"));
}

std::shared_ptr< GameObject > ObjectManager::findObject(GameObject::id_t id) const
{
    return m_objectsById.at(id);
}

void ObjectManager::addUpdatedObject(const std::shared_ptr< GameObject >& obj)
{
    m_alteredObjects.insert(obj);
}

void ObjectManager::flushUpdateCache()
{
    m_alteredObjects.clear();
    m_newObjects.clear();
    m_removedIds.clear();
}

void ObjectManager::serializeChanges(omsproto::GameObjectSet* object_set) const
{
    for(const auto& obj: m_newObjects) {
        obj->serialize(object_set->add_objects(), true);
    }
    for(const auto& obj: m_alteredObjects) {
        // Ff the object was newly created before, the changes are
        // already transmitted by the "new object" block, so we don't
        // need to re-serialize
        if(m_newObjects.find(obj) == m_newObjects.end()) {
            obj->serialize(object_set->add_objects(), false);
        }
    }
}

void ObjectManager::serializeFull(omsproto::GameObjectSet* object_set) const
{
    for(const auto& it: m_objectsById) {
        it.second->serialize(object_set->add_objects(), true);
    }
}

void ObjectManager::deserialize(omsproto::GameObjectSet* object_set)
{
    for(const auto& object: object_set->objects()) {
        const auto& it = m_objectsById.find(object.id());
        if(it == m_objectsById.end()) {
            auto newObject = std::make_shared<GameObject>(object.id(), object.name());
            m_objectsById[newObject->id()] = newObject;
            newObject->deserialize(&object);
        }
        else {
            m_objectsById[object.id()]->deserialize(&object);
        }
    }
}


}


