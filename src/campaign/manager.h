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

namespace campaign {

class ScriptEngine;

class Manager
{
public:
    Manager(const std::string& campaignName);
    ~Manager();

    bool loadCampaignPath();

    std::shared_ptr<ScriptEngine> scriptEngine() { return m_scriptEngine; }

private:
    static bool checkLocation(const std::string& campaignName, const boost::filesystem::path& location);

    std::shared_ptr<ScriptEngine> m_scriptEngine;
    std::string m_campaignName;
    boost::filesystem::path m_path;
};
}

#endif // CAMPAIGN_MANAGER_H
