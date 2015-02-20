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

#ifndef TERRAINLAYER_H
#define TERRAINLAYER_H

#include <string>

namespace Ogre {
    class Terrain;
}

class TerrainLayer
{
public:
    TerrainLayer();
    ~TerrainLayer();

    void setName(const std::string& name) { m_name = name; }
    std::string name() const { return m_name; }

    void setDiffuseTexture(const std::string& t) { m_diffuseTexture = t; }
    void setNormalMap(const std::string& t) { m_normalMap = t; }
    void setWorldScale(const std::string& t) { m_normalMap = t; }

    void applyLayer(Ogre::Terrain* terrain, int blendLayer);

public:
    std::string m_name;
    std::string m_diffuseTexture;
    std::string m_normalMap;
    float m_worldScale;
};

#endif // TERRAINLAYER_H
