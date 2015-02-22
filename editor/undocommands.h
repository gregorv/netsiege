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

#ifndef UNDO_HEIGHTMAPEDIT_H
#define UNDO_HEIGHTMAPEDIT_H

#include <QUndoCommand>
#include <OgreRectangle2D.h>

namespace Ogre {
    class TerrainGroup;
}

namespace undo {

class Command: public QUndoCommand
{
public:
    virtual void finalize() = 0;
};

class HeightmapEdit : public Command
{
public:
    HeightmapEdit(Ogre::TerrainGroup* terrainGroup);
    ~HeightmapEdit();

    virtual void finalize();
    void monitorTerrain(int x, int y, const Ogre::Rect& bounds);

    virtual void redo() { applyDeltas(true); }
    virtual void undo() { applyDeltas(false); }

private:
    struct _change_t {
        std::vector<float> delta_map;
        int x;
        int y;
        Ogre::Rect bounds;
    };
    std::vector<_change_t> m_changes;
    Ogre::TerrainGroup* m_terrainGroup;
    bool m_firstRedo;

    void applyDeltas(bool add);
};

class BlendmapEdit : public Command
{
public:
    BlendmapEdit(Ogre::TerrainGroup* terrainGroup, int layerIndex);
    ~BlendmapEdit();

    virtual void finalize();
    void monitorTerrain(int x, int y);

    virtual void redo() { applyDeltas(true); }
    virtual void undo() { applyDeltas(false); }

    int getLayerIndex() { return m_layerIndex; }

private:
    struct _change_t {
        std::vector<float> delta_map;
        int x;
        int y;
    };
    std::vector<_change_t> m_changes;
    Ogre::TerrainGroup* m_terrainGroup;
    int m_layerIndex;
    bool m_firstRedo;

    void applyDeltas(bool add);
};

}

#endif // UNDO_HEIGHTMAPEDIT_H
