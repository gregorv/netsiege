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

#include "gameobject.h"
#include "objects.pb.h"

#include <cassert>

namespace oms {

GameObject::GameObject(const std::string& name)
: m_name(name), m_isVisible(false)
{
    static id_t nextId = 0;
    m_id = nextId++;
}

GameObject::GameObject(id_t id, const std::string& name)
: GameObject(name)
{
    m_id = id;
}

void GameObject::serialize(omsproto::GameObject* object, bool forceFullSerialize) const
{
    assert(object);
    object->set_id(m_id);
    if(forceFullSerialize) {
        object->set_name(m_name);
    }
    object->set_is_visible(m_isVisible);
    object->clear_position();
    for(int8_t i=0; i<16; i++) {
        object->add_position((&m_positionMatrix[0][0])[i]);
    }
}

void GameObject::deserialize(const omsproto::GameObject* object)
{
    assert(object);
    m_id = object->id();
    if(object->has_name()) {
        m_name = object->name();
    }
    for(int8_t i=0; i<16; i++) {
        (&m_positionMatrix[0][0])[i] = object->position().Get(i);
    }
    m_isVisible = object->is_visible();
}

void GameObject::setPositionMatrix(const Ogre::Matrix4& pos)
{
    m_positionMatrix = pos;
}

}
