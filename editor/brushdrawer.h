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

#ifndef BRUSHDRAWER_H
#define BRUSHDRAWER_H

#include <QObject>
#include <OgreManualObject.h>

namespace Ogre {
    class TerrainGroup;
}

class BrushDrawer : public Ogre::ManualObject
{
public:
    BrushDrawer(const Ogre::String& name, Ogre::TerrainGroup* group);
    virtual const Ogre::String& getMovableType(void ) const;
    void setBrushOptions(float size, float hardness, float height);
    void setPosition(Ogre::Vector3 position);
    void showHeightMarker(bool show);
    
    void setTerrainGroup(Ogre::TerrainGroup* group) { m_group = group; }

private:
    void generateGeometry();
    Ogre::TerrainGroup* m_group;
    float m_outerRadius;
    float m_innterRadius;
    float m_height;
    Ogre::Vector3 m_position;
    bool m_marker;
};

#endif // BRUSHDRAWER_H
