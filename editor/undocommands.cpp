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

#include "undocommands.h"
#include "campaignmanager.h"

#include <QApplication>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <cassert>
#include <algorithm>
#include "campaignmanager.h"

using namespace undo;

HeightmapEdit::HeightmapEdit(Ogre::TerrainGroup* terrainGroup)
 : Command(),
 m_terrainGroup(terrainGroup), m_firstRedo(true)
{
    setText(QApplication::translate("EditorWindow", "modify heightmap", 0));
}

HeightmapEdit::~HeightmapEdit()
{
}

void HeightmapEdit::monitorTerrain(int x, int y, const Ogre::Rect& bounds)
{
    for(auto& change: m_changes) {
        if(x == change.x && y == change.y) {
            // terrain already monitored, extend rect
            change.bounds.top = std::min(change.bounds.top, bounds.top);
            change.bounds.left = std::min(change.bounds.left, bounds.left);
            change.bounds.right = std::max(change.bounds.right, bounds.right);
            change.bounds.bottom = std::max(change.bounds.bottom, bounds.bottom);
            return;
        }
    }
    CampaignManager::getOpenDocument()->setTileModified(x, y);
    _change_t change;
    auto terrain = m_terrainGroup->getTerrain(x, y);
    change.x = x;
    change.y = y;
    change.bounds = bounds;
    change.delta_map.resize(terrain->getSize() * terrain->getSize());
    memcpy(&change.delta_map[0], terrain->getHeightData(), change.delta_map.size()*sizeof(float));
    m_changes.push_back(change);
}

void HeightmapEdit::finalize()
{
    for(auto& change: m_changes) {
        auto terrain = m_terrainGroup->getTerrain(change.x, change.y);
        float* data = terrain->getHeightData();
        assert(terrain->getSize()*terrain->getSize() == change.delta_map.size());
        for(size_t i=0; i < change.delta_map.size(); i++) {
            change.delta_map[i] = data[i] - change.delta_map[i];
        }
    }
}

void HeightmapEdit::applyDeltas(bool add)
{
    if(m_firstRedo) {
        m_firstRedo = false;
        return;
    }
    for(auto& change: m_changes) {
        bool unload = false;
        CampaignManager::getOpenDocument()->setTileModified(change.x, change.y);
        auto ter = m_terrainGroup->getTerrain(change.x, change.y);
        if(!ter) {
            unload = true;
            m_terrainGroup->defineTerrain(change.x, change.y);
            m_terrainGroup->loadTerrain(change.x, change.y, true);
            ter = m_terrainGroup->getTerrain(change.x, change.y);
        }
        assert(ter->getSize()*ter->getSize() == change.delta_map.size());
        auto heightmap = ter->getHeightData();
        if(add) {
            for(size_t i=0; i<change.delta_map.size(); i++)
                heightmap[i] += change.delta_map[i];
        } else {
            for(size_t i=0; i<change.delta_map.size(); i++)
                heightmap[i] -= change.delta_map[i];
        }
        ter->dirtyRect(change.bounds);
        if(unload) {
            ter->update(true);
            ter->save(m_terrainGroup->generateFilename(change.x, change.y));
            ter->unload();
        } else {
            ter->update();
        }
    }
}



BlendmapEdit::BlendmapEdit(Ogre::TerrainGroup* terrainGroup, int layerIndex)
 : Command(),
 m_terrainGroup(terrainGroup), m_layerIndex(layerIndex), m_firstRedo(true)
{
    setText(QApplication::translate("EditorWindow", "blend layer modification", 0));
}

BlendmapEdit::~BlendmapEdit()
{
}

void BlendmapEdit::monitorTerrain(int x, int y)
{
    for(auto& change: m_changes) {
        if(x == change.x && y == change.y) {
            return;
        }
    }
    CampaignManager::getOpenDocument()->setTileModified(x, y);
    _change_t change;
    auto ter = m_terrainGroup->getTerrain(x, y);
    auto layer = ter->getLayerBlendMap(m_layerIndex);
    auto layerSize = ter->getLayerBlendMapSize();
    change.x = x;
    change.y = y;
    change.delta_map.resize(layerSize*layerSize);
    memcpy(&change.delta_map[0], layer->getBlendPointer(), change.delta_map.size()*sizeof(float));
    m_changes.push_back(change);
}

void BlendmapEdit::finalize()
{
    for(auto& change: m_changes) {
        auto terrain = m_terrainGroup->getTerrain(change.x, change.y);
        auto layer = terrain->getLayerBlendMap(m_layerIndex);
        auto layerSize = terrain->getLayerBlendMapSize();
        float* data = layer->getBlendPointer();
        assert(layerSize*layerSize == change.delta_map.size());
        for(size_t i=0; i < change.delta_map.size(); i++) {
            change.delta_map[i] = data[i] - change.delta_map[i];
        }
    }
}

void BlendmapEdit::applyDeltas(bool add)
{
    if(m_firstRedo) {
        m_firstRedo = false;
        return;
    }
    for(auto& change: m_changes) {
        bool unload = false;
        auto terrain = m_terrainGroup->getTerrain(change.x, change.y);
        CampaignManager::getOpenDocument()->setTileModified(change.x, change.y);
        if(!terrain) {
            unload = true;
            m_terrainGroup->defineTerrain(change.x, change.y);
            m_terrainGroup->loadTerrain(change.x, change.y, true);
            terrain = m_terrainGroup->getTerrain(change.x, change.y);
        }
        auto layer = terrain->getLayerBlendMap(m_layerIndex);
        auto layerSize = terrain->getLayerBlendMapSize();
        assert(layerSize*layerSize == change.delta_map.size());
        auto heightmap = layer->getBlendPointer();
        if(add) {
            for(size_t i=0; i<change.delta_map.size(); i++)
                heightmap[i] += change.delta_map[i];
        } else {
            for(size_t i=0; i<change.delta_map.size(); i++)
                heightmap[i] -= change.delta_map[i];
        }
        terrain->dirty();
        if(unload) {
            terrain->update(true);
            terrain->save(m_terrainGroup->generateFilename(change.x, change.y));
            terrain->unload();
        } else {
            terrain->update();
        }
    }
}

