/*
 * netsiege - A multiplayer point and click adventure
 * Copyright (C) 2015 Gregor Vollmer <gregor@celement.de>
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

#ifndef CAMPAIGNMANAGER_H
#define CAMPAIGNMANAGER_H

#include <string>
#include <memory>

namespace Ogre {
    class SceneManager;
    class TerrainGroup;
    class PageManager;
    class TerrainPaging;
}
class QUndoStack;

class CampaignManager
{
public:
    CampaignManager(const std::string& campaignPath);
    ~CampaignManager();

    void initialize(uint16_t terrainSize, float realWorldSize);
    void load();
    void save();

private:
    std::string m_campaignPath;
    Ogre::SceneManager* m_sceneManager;
    std::unique_ptr<Ogre::TerrainGroup> m_group;
    std::unique_ptr<Ogre::TerrainPaging> m_terrainPaging;
};

#endif // CAMPAIGNMANAGER_H
