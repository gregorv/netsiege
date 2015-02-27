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

#include "brushdrawer.h"
#include <cmath>
#include <Terrain/OgreTerrainGroup.h>

#define NUM_SEGMENTS 64
#define Y_OFFSET 0.1f

Ogre::String s_movableType("EditBrush");

BrushDrawer::BrushDrawer(const Ogre::String& name, Ogre::TerrainGroup* group)
: ManualObject(name), m_group(group), m_marker(false)
{
    begin("", Ogre::RenderOperation::OT_LINE_STRIP);
    for(size_t i=0; i<NUM_SEGMENTS; i++) {
        colour(0.2f, 1.0f, 0.2f, 1.0f);
        position(0.0f, 0.0f, 0.0f);
    }
    end();
    begin("", Ogre::RenderOperation::OT_LINE_STRIP);
    for(size_t i=0; i<NUM_SEGMENTS; i++) {
        colour(0.2f, 1.0f, 0.2f, 1.0f);
        position(0.0f, 0.0f, 0.0f);
    }
    end();
    begin("", Ogre::RenderOperation::OT_LINE_LIST);
    colour(0.2f, 1.0f, 0.2f, 1.0f);
    position(-0.5, 0.0, 0.0);
    colour(0.2f, 1.0f, 0.2f, 1.0f);
    position(0.5, 0.0, 0.0);

    colour(0.2f, 1.0f, 0.2f, 1.0f);
    position(0.0, 0.0, 0.5);
    colour(0.2f, 1.0f, 0.2f, 1.0f);
    position(0.0, 0.0, -0.5f);

    colour(0.2f, 1.0f, 0.2f, 1.0f);
    position(0.0, 0.0, 0.5);
    colour(0.2f, 1.0f, 0.2f, 1.0f);
    position(0.0, 0.0, -0.5f);
    end();
    generateGeometry();
}

const Ogre::String& BrushDrawer::getMovableType(void ) const
{
    return s_movableType;
}

void BrushDrawer::setBrushOptions(float size, float hardness, float height)
{
    m_outerRadius = size/2.0f;
    m_innterRadius = size*hardness/2.0f;
    m_height = height;
    generateGeometry();
}

void BrushDrawer::setPosition(Ogre::Vector3 position)
{
    m_position = position;
    generateGeometry();
}

void BrushDrawer::showHeightMarker(bool show)
{
    m_marker = show;
}

void BrushDrawer::generateGeometry()
{
    if(!m_group)
        return;
    beginUpdate(0);
    for(size_t i=0; i<NUM_SEGMENTS; i++) {
        colour(0.2f, 1.0f, 0.2f, 1.0f);
        float angle = 2.0f*3.1415f*i/(NUM_SEGMENTS-1);
        Ogre::Vector3 pos(sinf(angle)*m_outerRadius + m_position.x,
                          0.0f,
                          cosf(angle)*m_outerRadius + m_position.z);
        pos.y = m_group->getHeightAtWorldPosition(pos) + Y_OFFSET;
        position(pos);
    }
    end();

    beginUpdate(1);
    for(size_t i=0; i<NUM_SEGMENTS; i++) {
        colour(0.2f, 1.0f, 0.2f, 1.0f);
        float angle = 2.0f*3.1415f*i/(NUM_SEGMENTS-1);
        Ogre::Vector3 pos(sinf(angle)*m_innterRadius + m_position.x,
                          0.0f,
                          cosf(angle)*m_innterRadius + m_position.z);
        pos.y = m_group->getHeightAtWorldPosition(pos) + Y_OFFSET;
        position(pos);
    }
    end();

    float terrain_height = m_group->getHeightAtWorldPosition(m_position) + Y_OFFSET;
    float cross_height = m_marker? m_height + Y_OFFSET : terrain_height;
    beginUpdate(2);
    colour(0.2f, 1.0f, 0.2f, 1.0f);
    position(-0.5 + m_position.x, cross_height, m_position.z);
    colour(0.2f, 1.0f, 0.2f, 1.0f);
    position(0.5 + m_position.x,cross_height, m_position.z);
    colour(0.2f, 1.0f, 0.2f, 1.0f);
    position(0.0 + m_position.x, cross_height, 0.5 + m_position.z);
    colour(0.2f, 1.0f, 0.2f, 1.0f);
    position(0.0 + m_position.x, cross_height, -0.5f + m_position.z);

    colour(0.2f, 1.0f, 0.2f, 1.0f);
    position(m_position.x, terrain_height, m_position.z);
    colour(0.2f, 1.0f, 0.2f, 1.0f);
    position(m_position.x, cross_height, m_position.z);
    end();
}
