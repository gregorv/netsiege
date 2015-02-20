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

#include "terrainlayer.h"
#include <Terrain/OgreTerrain.h>

TerrainLayer::TerrainLayer()
 : m_name()
{

}

TerrainLayer::~TerrainLayer()
{

}

void TerrainLayer::applyLayer(Ogre::Terrain* terrain, int blendLayer)
{
    terrain->setLayerWorldSize(blendLayer, m_worldScale);
    terrain->setLayerTextureName(blendLayer, 0, m_diffuseTexture.c_str());
    terrain->setLayerTextureName(blendLayer, 1, m_normalMap.c_str());
}
