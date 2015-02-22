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

#ifndef BRUSH_H
#define BRUSH_H

#include <QObject>
#include <OgreRay.h>

namespace Ogre { class TerrainGroup; class SceneManager; }
class QUndoStack;
class BrushDrawer;
namespace undo { class Command; }

class Brush : public QObject
{
    Q_OBJECT
public:
    enum mode_t {
        M_NONE = 0,
        M_HEIGHT = 1,
        M_HEIGHT_RAISE = 1+2,
        M_HEIGHT_SMOOTH = 1+4,
        M_HEIGHT_FLATTEN = 1+8,
        M_LAYER_PAINT = 16
    };

    Brush(Ogre::TerrainGroup* group, Ogre::SceneManager* sceneMgr, QUndoStack* stack = nullptr);
    ~Brush();

    void setUndoStack(QUndoStack* stack) { stopStroke(); m_stack = stack; }
    void setTerrainGroup(Ogre::TerrainGroup* group);

    void setMode(mode_t mode);
    void startStroke();
    void update(const Ogre::Ray& pickray);
    void pointerUpdate(const Ogre::Ray& pickray);
    void stopStroke();

    void setEnabled(bool enabled);
    bool enabled() const { return m_enabled; }

    BrushDrawer* getDrawer() { return m_drawer; }

public slots:
    void setStrength(float x);
    void setSize(float x);
    void setHardness(float x);

    void setStrength(int x);
    void setSize(int x);
    void setHardness(int x);

    void setBlendLayer(int layer);

signals:
    void strengthChanged(float x);
    void sizeChanged(float x);
    void hardnessChanged(float x);

    void strengthChangedInt(int x);
    void sizeChangedInt(int x);
    void hardnessChangedInt(int x);

protected:
    struct settings_t {
        float strength;
        float size;
        float hardness;
    };
    void pointerUpdate(bool hit, const Ogre::Vector3& position);
    void applyHeightBrush(long x, long y, const Ogre::Vector3& position);
    void applyLayerBrush(long x, long y, const Ogre::Vector3& position);
    mode_t m_mode;
    float m_height;
    QUndoStack* m_stack;
    BrushDrawer* m_drawer;
    Ogre::TerrainGroup* m_group;
    undo::Command* m_currentCommand;
    std::map<mode_t, settings_t> m_settings;
    int m_blendLayer;
    bool m_enabled;
};

#endif // BRUSH_H
