/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2015  <copyright holder> <email>
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

#ifndef OGREBASE_H
#define OGREBASE_H

#include "iogreeventhandler.hpp"
#include <OgreVector2.h>

namespace Ogre {
    class SceneManager;
    class PageManager;
    class TerrainPaging;
    class TerrainPagedWorldSection;
}
class EditorCamera;
class Brush;
class QUndoStack;
class QTimer;
class QWidget;

class OgreBase : public IOgreEventHandler
{
public:
    OgreBase(QWidget* window);
    ~OgreBase();
    virtual void ogreKeyReleaseEvent(QKeyEvent* event);
    virtual void ogreKeyPressEvent(QKeyEvent* event);
    virtual void ogreMouseMoveEvent(QMouseEvent* event);
    virtual void ogreMouseReleaseEvent(QMouseEvent* event);
    virtual void ogreMousePressEvent(QMouseEvent* event);
    virtual void ogreUpdate();
    virtual void ogreResizeEvent(QResizeEvent* rEvent);
    virtual void ogrePaintEvent(QPaintEvent* pEvent);

    static OgreBase* getSingletonPtr() { return s_singleton; }
    static OgreBase& getSingleton() { return *s_singleton; }

    Ogre::SceneManager* getSceneManager() const { return m_sceneManager; }
    Ogre::PageManager* getPageManager() const { return m_pageManager; }
    Ogre::TerrainPaging* getTerrainPaging() const { return m_terrainPaging; }
    Brush* getBrush() const { return m_brush; }
    EditorCamera* getCamera() const { return m_camera; }
    QUndoStack* getStack() const { return m_stack; }


private:
    void viewUpdate();

    Ogre::Vector2 m_prevMousePosition;

    QWidget* m_window;

    QTimer* m_updateTimer;
    QUndoStack* m_stack;

    Ogre::SceneManager* m_sceneManager;
    Ogre::PageManager* m_pageManager;
    Ogre::TerrainPaging* m_terrainPaging;
    EditorCamera* m_camera;
    Brush* m_brush;
    static OgreBase* s_singleton;
};

#endif // OGREBASE_H
