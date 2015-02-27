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

#include "ogrebase.h"
#include "editorcamera.h"
#include "brush.h"
#include "editableterrainpagedworldsection.h"

#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgrePageManager.h>
#include <OgreTerrain.h>
#include <OgreTerrainPaging.h>

#include <QTimer>
#include <QApplication>
#include <QCursor>
#include <QUndoStack>
#include <QWidget>
#include <QMouseEvent>


OgreBase* OgreBase::s_singleton = nullptr;

OgreBase::OgreBase(QWidget* window)
: m_window(window), m_stack(new QUndoStack)
{
    if(s_singleton) {
        throw std::runtime_error("Created second OgreBase object! Supposed to be singleton...");
    }
    s_singleton = this;
    m_sceneManager = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC, "");
    m_camera = new EditorCamera(m_sceneManager);
    m_brush = new Brush(nullptr, m_sceneManager, m_stack);

    float interval_ms = 25.0f;
    m_updateTimer = new QTimer();
    m_updateTimer->setInterval(interval_ms);
    m_updateTimer->setSingleShot(false);
    m_updateTimer->start();
    QObject::connect(m_updateTimer, &QTimer::timeout, [=]() { this->viewUpdate(); });
    m_camera->setUpdateInterval(interval_ms/1000.0f);

    auto globalOptions = OGRE_NEW Ogre::TerrainGlobalOptions();
    globalOptions->setMaxPixelError(8);
    globalOptions->setCompositeMapAmbient(Ogre::ColourValue(0.1f, 0.1f, 0.1f, 1.0f));
    globalOptions->setCastsDynamicShadows(true);
//     globalOptions->setLightMapDirection(lightDir);
    globalOptions->setCompositeMapDistance(1.0f);
    globalOptions->setCompositeMapSize(128);
    globalOptions->setSkirtSize(2.0f);


    m_pageManager = new Ogre::PageManager;
    m_pageManager->addCamera(m_camera->getCamera());
    m_pageManager->setDebugDisplayLevel(100);
    m_pageManager->addWorldSectionFactory(new EditableTerrainPagedWorldSection::SectionFactory);
    m_terrainPaging = new Ogre::TerrainPaging(m_pageManager);
}

OgreBase::~OgreBase()
{
    delete m_updateTimer;
    delete m_camera;
    delete m_brush;
}

void OgreBase::ogreKeyReleaseEvent(QKeyEvent* event)
{
    m_camera->keyRelease(event);
}

void OgreBase::ogreKeyPressEvent(QKeyEvent* event)
{
    m_camera->keyPress(event);
}

void OgreBase::ogreMouseMoveEvent(QMouseEvent* event)
{

}

void OgreBase::ogreMouseReleaseEvent(QMouseEvent* event)
{
    m_camera->mouseRelease(event);
    if(event->button() == Qt::LeftButton) {
        m_brush->stopStroke();
//         m_group->getTerrain(0, 0)->dirty();
//         m_group->getTerrain(0, 0)->dirtyLightmap();
//         m_group->getTerrain(0, 0)->update();
    }
}

void OgreBase::ogreMousePressEvent(QMouseEvent* event)
{
    m_camera->mousePress(event);
    if(event->button() == Qt::LeftButton) {
        m_brush->startStroke();
//         m_group->getTerrain(0, 0)->dirty();
//         m_group->getTerrain(0, 0)->dirtyLightmap();
//         m_group->getTerrain(0, 0)->update();
    }
}

void OgreBase::ogreUpdate()
{

}

void OgreBase::ogreResizeEvent(QResizeEvent* rEvent)
{

}

void OgreBase::ogrePaintEvent(QPaintEvent* pEvent)
{

}

void OgreBase::viewUpdate()
{
    auto globalPoint = QCursor::pos();
    auto localPoint = m_window->mapFromGlobal(globalPoint);
    Ogre::Vector2 globalPos(globalPoint.x(), globalPoint.y());
    Ogre::Vector2 pos(localPoint.x(), localPoint.y());
    Ogre::Vector2 delta(Ogre::Vector2::ZERO);
    if(pos != m_prevMousePosition) {
        if(QApplication::mouseButtons() != Qt::NoButton &&
           QApplication::mouseButtons() != Qt::LeftButton) {
            delta = pos - m_prevMousePosition;
            bool anyChange = false;
            if(pos.x < 0) {
                pos.x += m_window->width();
                globalPos.x += m_window->width();
                anyChange = true;
            } else if(pos.x > m_window->width()) {
                pos.x -= m_window->width();
                globalPos.x -= m_window->width();
                anyChange = true;
            }
            if(pos.y< 0) {
                pos.y += m_window->height();
                globalPos.y += m_window->height();
                anyChange = true;
            } else if(pos.y > m_window->height()) {
                pos.y -= m_window->height();
                globalPos.y -= m_window->height();
                anyChange = true;
            }
            if(anyChange)
                QCursor::setPos(globalPos.x, globalPos.y);
        }
        m_prevMousePosition = pos;
        m_camera->mouseMove(pos/Ogre::Vector2(m_window->width(), m_window->height()), delta);
        if(QApplication::mouseButtons() & Qt::LeftButton) {
            m_brush->update(m_camera->getPickRay());
        } else  {
            m_brush->pointerUpdate(m_camera->getPickRay());
        }
    }
    m_camera->update();
}


