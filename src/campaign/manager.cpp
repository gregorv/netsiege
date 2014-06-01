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

#include "manager.h"
#include "script/scriptengine.h"
#include "oms/objectmanager.h"
#include "debug/ndebug.h"
#include <list>
#include <OGRE/OgreResourceGroupManager.h>

using namespace campaign;
using namespace boost::filesystem;

Manager::Manager(const std::string& campaignName)
 : m_scriptEngine(std::make_shared<script::ScriptEngine>()),
 m_objectManager(std::make_shared<oms::ObjectManager>()),
 m_campaignName(campaignName)
{
    Ogre::ResourceGroupManager::getSingleton().createResourceGroup(campaignName);
}

Manager::~Manager()
{

}

bool Manager::loadCampaignPath()
{
    std::list<path> searchPaths;
    auto cwd = current_path();
    cwd /= "campaigns";
    searchPaths.push_back(cwd);
    auto envCampaign = getenv("CAMPAIGN_PATH");
    if(envCampaign) {
        searchPaths.push_back(path(envCampaign));
    }
    for(auto searchPath: searchPaths) {
        nDebug << "Search for campaign '" << m_campaignName << "' in directory " << searchPath << std::endl;
        if(!exists(searchPath)) {
            continue;
        }
        auto dirPath = searchPath;
        dirPath /= m_campaignName;
        auto zipPath = searchPath;
        zipPath /= (m_campaignName+".zip");
        if(exists(dirPath) && is_directory(dirPath)) {
            m_path = dirPath;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(m_path.c_str(), "FileSystem", m_campaignName);
            return true;
        }
        if(exists(zipPath) && is_regular_file(zipPath)) {
            m_path = zipPath;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(m_path.c_str(), "Zip", m_campaignName);
            return true;
        }
    }
    return false;
}

bool Manager::checkLocation(const std::string& campaignName, const path& location)
{
    if(!exists(location)) {
        return false;
    }
    return true;
}

