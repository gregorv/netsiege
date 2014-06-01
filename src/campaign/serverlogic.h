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

#ifndef CAMPAIGN_SERVERLOGIC_H
#define CAMPAIGN_SERVERLOGIC_H

#include <memory>

namespace campaign {

class Manager;

const std::string SERVER_AS_MODULE("server");

class ServerLogic
{
public:
    ServerLogic(const std::shared_ptr<Manager>& manager);
    ~ServerLogic();

    bool init();
    bool step(float dt);

private:
    std::shared_ptr<Manager> m_manager;
};
}

#endif // CAMPAIGN_SERVERLOGIC_H
