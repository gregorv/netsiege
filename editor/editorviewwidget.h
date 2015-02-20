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

#ifndef OGREEDITORWIDGET_H
#define OGREEDITORWIDGET_H

#include <QWidget>

namespace Ogre
{
    class Root;
    class SceneManager;
    class SceneNode;
    class Terrain;
    class TerrainGroup;
    class TerrainGlobalOptions;
}
class QUndoStack;
class Brush;
class QOgreWidget;
namespace undo { class Command; }

class EditorViewWidget : public QWidget
{
    Q_OBJECT
public:
    EditorViewWidget(QWidget* parent = 0);
    ~EditorViewWidget();


protected:
    void setupWidgets();

    Ogre::Root* m_root;
    QOgreWidget* m_widget;
    Ogre::Terrain* m_terrain;
    Ogre::TerrainGroup* m_group;
    Ogre::TerrainGlobalOptions* m_globalOptions;
    QTimer* m_motionTimer;
};

#endif // OGREEDITORWIDGET_H
