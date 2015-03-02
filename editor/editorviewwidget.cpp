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

#include "editorviewwidget.h"
#include "undocommands.h"
#include "brush.h"
#include "qogrewidget.hpp"
#include "ogrebase.h"
#include "editorcamera.h"

#include <cmath>

#include <QApplication>
#include <QUndoStack>
#include <QTimer>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QKeyEvent>

#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreRenderWindow.h>
#include <OgreEntity.h>
#include <OgreMovableObject.h>
#include <OgreRoot.h>
#include <OgreTextureManager.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

EditorViewWidget::EditorViewWidget(QWidget *parent)
 : QWidget(parent),
   m_root(new Ogre::Root("plugins.cfg", "ogre.cfg", "ogre.log")),
   m_widget(nullptr),
   m_terrain(nullptr),
   m_globalOptions(nullptr)
{
    m_root->restoreConfig();
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../testmedia/", "FileSystem", "");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../testmedia/p0ss/", "FileSystem", "");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../testmedia/dim/", "FileSystem", "");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("testmedia/", "FileSystem", "");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("testmedia/p0ss/", "FileSystem", "");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("testmedia/dim/", "FileSystem", "");
    Ogre::RenderSystemList::const_iterator availableRendererIt = m_root->getAvailableRenderers().begin();
    m_root->setRenderSystem(*availableRendererIt);
    m_root->initialise(false);
    setupWidgets();
    new OgreBase(m_widget);
    m_widget->setEventHandler(OgreBase::getSingletonPtr());

    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(3);

    auto sceneManager = OgreBase::getSingleton().getSceneManager();
    sceneManager->setSkyDome(true, "CloudySky", 5, 8);
    sceneManager->setAmbientLight(Ogre::ColourValue(0.2f, 0.2f, 0.2f));
    Ogre::Vector3 lightDir(1.0, -2.0, 0.5);
    lightDir.normalise();
    auto lamp = sceneManager->createLight();
    lamp->setPosition(5, 5, 5);
    lamp->setCastShadows(true);
    lamp->setDirection(lightDir);
    lamp->setType(Ogre::Light::LT_DIRECTIONAL);
    lamp->setDiffuseColour(Ogre::ColourValue::White);
    lamp->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));
    Ogre::SceneNode *lightNode = sceneManager->getRootSceneNode()->createChildSceneNode("mainlight");
    lightNode->attachObject(lamp);
    m_widget->getEmbeddedOgreWindow()->addViewport(OgreBase::getSingleton().getCamera()->getCamera());
}

EditorViewWidget::~EditorViewWidget()
{
    delete OgreBase::getSingletonPtr();
    delete m_terrain;
    delete m_root;
    if(m_widget) delete m_widget;
}

void EditorViewWidget::setupWidgets()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    m_widget = new QOgreWidget(m_root, OgreBase::getSingletonPtr(), this);
    mainLayout->addWidget(m_widget);
    this->setLayout(mainLayout);
}

