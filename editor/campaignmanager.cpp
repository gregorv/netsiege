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
#include "editableterrainpagedworldsection.h"
#include <boost/filesystem.hpp>
#include <QApplication>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreResourceGroupManager.h>
#include <OgrePagedWorld.h>
#include <OgreTerrainGroup.h>
#include <OgreTerrainPaging.h>
#include <OgreTerrainPagedWorldSection.h>
#include <OgreGrid2DPageStrategy.h>


class TerrainDefiner : public Ogre::TerrainPagedWorldSection::TerrainDefiner
{
public:
    virtual void define(Ogre::TerrainGroup* terrainGroup, long x, long y)
    {
        std::string filename(terrainGroup->generateFilename(x, y));
        std::string tmpfile("~"+filename);
        auto campaignManager = CampaignManager::getOpenDocument();
        if(campaignManager->isSlotDefined(x, y)) {
            if(boost::filesystem::exists(tmpfile)) {
                terrainGroup->defineTerrain(x, y, tmpfile);
            } else if(boost::filesystem::exists(filename)) {
                terrainGroup->defineTerrain(x, y);
            } else {
                terrainGroup->defineTerrain(x, y, 0.0f);
            }
        }
    }
} g_terrainDefiner;

CampaignManager* CampaignManager::s_openDocument = nullptr;

CampaignManager::CampaignManager(const std::string& campaignPath)
: m_campaignPath(campaignPath)
{
    if(s_openDocument) {
        throw std::runtime_error("Cannot open more than one campaign at once!");
    }
    s_openDocument = this;
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(m_campaignPath.c_str(), "FileSystem", "campaign");
    m_sceneManager = OgreBase::getSingleton().getSceneManager();
    m_group.reset(new Ogre::TerrainGroup(m_sceneManager, Ogre::Terrain::ALIGN_X_Z, 127, 30.0f));
    m_group->setFilenameConvention("qwertz", ".ter");
//     m_group->
    auto terrainPaging = OgreBase::getSingleton().getTerrainPaging();
    auto pageManager = OgreBase::getSingleton().getPageManager();
    m_pagedWorld.reset(pageManager->createWorld());
//     m_pagedTerrain.reset(terrainPaging->createWorldSection(m_pagedWorld.get(),
//                                                            m_group.get(),
//                                                            m_group->getTerrainSize(),
//                                                            m_group->getTerrainSize()*2,
//                                                            -10, -10, 10, 10, "",  50
//                                                           ));
    auto worldSection = m_pagedWorld->createSection(OgreBase::getSingleton().getSceneManager(), "EditTerrain", Ogre::String(""));
    m_pagedTerrain.reset(dynamic_cast<EditableTerrainPagedWorldSection*>(worldSection));
    m_pagedTerrain->init(m_group.get());
    m_pagedTerrain->setLoadRadius(m_group->getTerrainSize()*4);
    m_pagedTerrain->setHoldRadius(m_group->getTerrainSize()*5);
    m_pagedTerrain->setPageRange(-10, -10, 10, 10);
    m_pagedTerrain->setLoadingIntervalMs(10);
    m_pagedTerrain->setDefiner(&g_terrainDefiner);
    OgreBase::getSingleton().getBrush()->setTerrainGroup(m_group.get());
    Ogre::Terrain::ImportData& defaultimp = m_group->getDefaultImportSettings();
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;
    defaultimp.constantHeight = 0.0f;

    defaultimp.layerList.resize(5);
    defaultimp.layerList[0].worldSize = 10;
    defaultimp.layerList[0].textureNames.push_back("checkers.png");
    defaultimp.layerList[0].textureNames.push_back("checkers_normal.png");

    defaultimp.layerList[1].worldSize = 1;
    defaultimp.layerList[1].textureNames.push_back("tilable-IMG_0044-grey.png");
//     defaultimp.layerList[1].textureNames.push_back("tilable-IMG_0044-grey_normalmap.png");
    defaultimp.layerList[1].textureNames.push_back("neutral_normals.png");

    defaultimp.layerList[2].worldSize = 1;
    defaultimp.layerList[2].textureNames.push_back("CliffRock.JPG");
    defaultimp.layerList[2].textureNames.push_back("CliffRock_N.jpg");

    defaultimp.layerList[3].worldSize = 5;
    defaultimp.layerList[3].textureNames.push_back("Dirt.JPG");
    defaultimp.layerList[3].textureNames.push_back("Dirt_N.jpg");

    defaultimp.layerList[4].worldSize = 5;
    defaultimp.layerList[4].textureNames.push_back("CliffRock.JPG");
    defaultimp.layerList[4].textureNames.push_back("CliffRock_N.jpg");

//     defaultimp.layerList[5].worldSize = 5;
//     defaultimp.layerList[5].textureNames.push_back("Ancient Flooring.JPG");
//     defaultimp.layerList[5].textureNames.push_back("Ancient Flooring_N.jpg");

//     defaultimp.layerList[6].worldSize = 5;
//     defaultimp.layerList[6].textureNames.push_back("Boards.JPG");
//     defaultimp.layerList[6].textureNames.push_back("Boards_N.jpg");
//
//     defaultimp.layerList[7].worldSize = 5;
//     defaultimp.layerList[7].textureNames.push_back("Chimeny.JPG");
//     defaultimp.layerList[7].textureNames.push_back("Chimeny_N.jpg");
}

CampaignManager::~CampaignManager()
{
    s_openDocument = nullptr;
    Ogre::ResourceGroupManager::getSingleton().removeResourceLocation(m_campaignPath.c_str(), "campaign");
}

void CampaignManager::initialize(uint16_t terrainSize, float realWorldSize)
{
    Ogre::Terrain::ImportData& defaultimp = m_group->getDefaultImportSettings();
    defaultimp.terrainSize = terrainSize;
    defaultimp.worldSize = realWorldSize;
    m_group->setTerrainSize(terrainSize);
    m_group->setTerrainWorldSize(realWorldSize);
    m_definedTerrainSlots.push_back(m_group->packIndex(0, 0));
    m_definedTerrainSlots.push_back(m_group->packIndex(1, 0));
    m_definedTerrainSlots.push_back(m_group->packIndex(0, 1));
    m_definedTerrainSlots.push_back(m_group->packIndex(-1, 0));
    m_definedTerrainSlots.push_back(m_group->packIndex(-2, 0));
    m_definedTerrainSlots.push_back(m_group->packIndex(-3, 0));
    m_definedTerrainSlots.push_back(m_group->packIndex(-4, 0));
    m_definedTerrainSlots.push_back(m_group->packIndex(-5, 0));
    m_definedTerrainSlots.push_back(m_group->packIndex(-6, 0));
    updateSlotBoundary();
//     m_group->defineTerrain(0, 0, 0.0f);
//     m_group->defineTerrain(1, 0, 0.0f);
//     m_group->defineTerrain(0, 1, 0.0f);
//     m_group->defineTerrain(-1, 0, 0.0f);
//     m_group->defineTerrain(-2, 0, 0.0f);
//     m_group->defineTerrain(-3, 0, 0.0f);
//     m_group->defineTerrain(-4, 0, 0.0f);
//     m_group->defineTerrain(-5, 0, 0.0f);
//     m_group->defineTerrain(-6, 0, 0.0f);
    std::cout << "Load" << std::endl;
//     m_group->loadAllTerrains(true);
    std::cout << "Updage" << std::endl;
//     m_group->update(true);
    std::cout << "Save" << std::endl;
//     m_group->saveAllTerrains(false);
    std::cout << "Done" << std::endl;
    m_pagedTerrain->getGridStrategyData()->setCellSize(realWorldSize);
//     m_group->unload();
}

void CampaignManager::load()
{

}

void CampaignManager::save()
{
    emit initProgress(QApplication::translate("CampaignManager", "Waiting for terrain updates", 0).toStdString(), 0, true);
    while(m_group->isDerivedDataUpdateInProgress()) {
            OGRE_THREAD_SLEEP(25);
            QCoreApplication::processEvents();
    }
    emit initProgress(QApplication::translate("CampaignManager", "Save terrain", 0).toStdString(), m_editedTiles.size(), true);
    for(auto& edited: m_editedTiles) {
        auto ter = m_group->getTerrain(edited.x, edited.y);
        std::string filename = m_group->generateFilename(edited.x, edited.y);
        if(ter) {
            ter->save(filename);
            if(boost::filesystem::exists("~"+filename)) {
                boost::filesystem::remove("~"+filename);
            }
        } else if(boost::filesystem::exists("~"+filename)) {
            if(boost::filesystem::exists(filename)) {
                boost::filesystem::remove(filename);
            }
            boost::filesystem::rename("~"+filename, filename);
        }
        emit stepProgress();
    }
    m_editedTiles.clear();
    emit doneProgress();
}

void CampaignManager::clearAllTileModified()
{
    m_editedTiles.clear();
}

void CampaignManager::clearTileModified(long int x, long int y)
{
    for(size_t i=0; i<m_editedTiles.size(); i++) {
        if(m_editedTiles[i].x == x && m_editedTiles[i].y == y) {
            m_editedTiles.erase(m_editedTiles.begin()+i);
            return;
        }
    }
}

const std::vector< CampaignManager::edited_tile_t >& CampaignManager::getTileModified() const
{
    return m_editedTiles;
}

bool CampaignManager::isTileModified(long int x, long int y) const
{
    for(auto& it: m_editedTiles) {
        if(it.x == x && it.y == y) {
            return true;
        }
    }
    return false;
}

void CampaignManager::setTileModified(long int x, long int y)
{
    if(!isTileModified(x, y)) {
        edited_tile_t t { x, y, false };
        m_editedTiles.push_back(t);
    }
}

void CampaignManager::setTileUnpaged(long int x, long int y)
{
    for(auto& it: m_editedTiles) {
        if(it.x == x && it.y == y) {
            it.unpaged = true;
        }
    }
}

bool CampaignManager::isTileUnpaged(long int x, long int y) const
{
    for(auto& it: m_editedTiles) {
        if(it.x == x && it.y == y) {
            return it.unpaged;
        }
    }
    return false;
}

bool CampaignManager::isSlotDefined(long int x, long int y) const
{
    auto packedIdx = m_group->packIndex(x, y);
    for(const auto& slotIdx: m_definedTerrainSlots) {
        if(slotIdx == packedIdx) {
            return true;
        }
    }
    return false;
}

Ogre::Rect CampaignManager::getSlotBoundary() const
{
    return m_slotBoundary;
}

void CampaignManager::updateSlotBoundary()
{
    if(m_definedTerrainSlots.size() == 0) {
        m_slotBoundary = Ogre::Rect(0, 0, 0, 0);
    } else {
        long x, y;
        m_group->unpackIndex(m_definedTerrainSlots[0], &x, &y);
        m_slotBoundary = Ogre::Rect(x, y, x, y);
        for(auto& idx: m_definedTerrainSlots) {
            m_group->unpackIndex(idx, &x, &y);
            if(m_slotBoundary.left > x) m_slotBoundary.left = x;
            if(m_slotBoundary.right < x) m_slotBoundary.right = x;
            if(m_slotBoundary.top > y) m_slotBoundary.top = y;
            if(m_slotBoundary.bottom < y) m_slotBoundary.bottom = y;
        }
    }
}

void CampaignManager::unpackSlotIndex(uint32_t key, long* x, long* y) const
{
    m_group->unpackIndex(key, x, y);
}

