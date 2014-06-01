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

#ifndef CAMPAIGN_MANAGER_H
#define CAMPAIGN_MANAGER_H

#include <memory>
#include <boost/filesystem.hpp>

namespace script {
    class ScriptEngine;
}

namespace oms {
    class ObjectManager;
}

namespace campaign {

class Manager
{
public:
    Manager(const std::string& campaignName);
    ~Manager();

    bool loadCampaignPath();

    std::shared_ptr<script::ScriptEngine> scriptEngine() { return m_scriptEngine; }
    std::shared_ptr<oms::ObjectManager> objectManager() { return m_objectManager; }

private:
    static bool checkLocation(const std::string& campaignName, const boost::filesystem::path& location);

    std::shared_ptr<script::ScriptEngine> m_scriptEngine;
    std::shared_ptr<oms::ObjectManager> m_objectManager;
    std::string m_campaignName;
    boost::filesystem::path m_path;
};
}

#endif // CAMPAIGN_MANAGER_H
