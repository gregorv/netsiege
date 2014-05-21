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

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <OgreMatrix4.h>
#include <vector>
#include <ios>
#include <memory>
#include <string>


namespace omsproto {
    class GameObject;
}

namespace oms {

class GameObject
{
public:
    typedef uint32_t id_t;

    GameObject(const std::string& name);
    GameObject(id_t id, const std::string& name);
    void serialize(omsproto::GameObject* object, bool forceFullSerialize) const;
    void deserialize(const omsproto::GameObject* object);

    id_t id() const { return m_id; }
    std::string name() const { return m_name; }
    Ogre::Matrix4 positionMatrix() const { return m_positionMatrix; }
    void setPositionMatrix(const Ogre::Matrix4& pos);

    bool operator<(const GameObject& other) const { return id() < other.id(); }
    bool operator<(std::shared_ptr<GameObject> other) const { return id() < other->id(); }

private:
    const id_t m_id;
    static id_t nextId;
    const std::string m_name;
    Ogre::Matrix4 m_positionMatrix;
    bool m_isVisible = false;
};

}

#endif // GAMEOBJECT_H
