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

#include "campaignmanager.h"
#include "ogrebase.h"
#include "brush.h"
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreResourceGroupManager.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainPaging.h>
#include <Paging/OgrePageManager.h>

CampaignManager::CampaignManager(const std::string& campaignPath)
: m_campaignPath(campaignPath)
{
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(m_campaignPath.c_str(), "FileSystem", "campaign");
    m_sceneManager = OgreBase::getSingleton().getSceneManager();
    m_group.reset(new Ogre::TerrainGroup(m_sceneManager, Ogre::Terrain::ALIGN_X_Z, 127, 127.0f/4.0f));
    m_group->setFilenameConvention("/home/gregor/projekte/netsiege/testmedia/world/terrain/", ".ter");
    OgreBase::getSingleton().getBrush()->setTerrainGroup(m_group.get());
}

CampaignManager::~CampaignManager()
{
    Ogre::ResourceGroupManager::getSingleton().removeResourceLocation(m_campaignPath.c_str(), "campaign");
}

void CampaignManager::initialize(uint16_t terrainSize, float realWorldSize)
{

}

void CampaignManager::load()
{

}

void CampaignManager::save()
{

}

