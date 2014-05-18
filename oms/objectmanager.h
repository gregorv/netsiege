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
#include <string>
#include <memory>

#include "gameobject.h"

namespace oms {

class ObjectManager
{
public:
    ObjectManager();

    const std::shared_ptr<GameObject>& findObject(std::string name) const;
    const std::shared_ptr<GameObject>& findObject(GameObject::id_t id) const;

private:
    std::list<std::shared_ptr<GameObject>> m_alteredObjects;
    std::map<std::string, std::shared_ptr<GameObject>> m_objectsByName;
};

}

#endif // OBJECTMANAGER_H
