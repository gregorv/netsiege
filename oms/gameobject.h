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

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <OgreMatrix4.h>
#include <vector>
#include <ios>
#include <memory>
#include <string>

namespace oms {

class IComponent;

class GameObject
{
public:
    typedef uint32_t id_t;

    GameObject(const std::string& name);
    std::ios& serialize(std::ios& stream) const;
    std::ios& deserialize(std::ios& stream) const;

    id_t getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    Ogre::Matrix4& getPositionMatrix() { return m_positionMatrix; }
    const Ogre::Matrix4& getPositionMatrix() const { return m_positionMatrix; }

private:
    id_t m_id;
    std::string m_name;
    Ogre::Matrix4 m_positionMatrix;
    std::vector<std::shared_ptr<IComponent>> m_components;
};

}

#endif // GAMEOBJECT_H
