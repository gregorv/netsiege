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
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(3);
    setupWidgets();
    new OgreBase(m_widget);
    m_widget->setEventHandler(OgreBase::getSingletonPtr());

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

    m_globalOptions = OGRE_NEW Ogre::TerrainGlobalOptions();
    m_globalOptions->setMaxPixelError(8);
    m_globalOptions->setCompositeMapAmbient(Ogre::ColourValue(0.1f, 0.1f, 0.1f, 1.0f));
    m_globalOptions->setCastsDynamicShadows(true);
    m_globalOptions->setLightMapDirection(lightDir);
    m_globalOptions->setCompositeMapDistance(3000);
    m_globalOptions->setSkirtSize(2);

    m_group = OGRE_NEW Ogre::TerrainGroup(sceneManager, Ogre::Terrain::ALIGN_X_Z, 513, 513.0f/4);
    m_group->setFilenameConvention(Ogre::String("tile"), Ogre::String("ter"));
    m_group->setOrigin(Ogre::Vector3::ZERO);

    Ogre::Terrain::ImportData& defaultimp = m_group->getDefaultImportSettings();
    defaultimp.terrainSize = 513;
    defaultimp.worldSize = 513.0f/4;
    defaultimp.inputScale = 255/16; // due terrain.png is 8 bpp
    defaultimp.minBatchSize = 33;
    defaultimp.maxBatchSize = 65;
    defaultimp.constantHeight = 0.0f;

    defaultimp.layerList.resize(4);
    defaultimp.layerList[0].worldSize = 10;
    defaultimp.layerList[0].textureNames.push_back("checkers.png");
    defaultimp.layerList[0].textureNames.push_back("checkers_normal.png");

    defaultimp.layerList[1].worldSize = 1;
    defaultimp.layerList[1].textureNames.push_back("tilable-IMG_0044-grey.png");
//     defaultimp.layerList[1].textureNames.push_back("tilable-IMG_0044-grey_normalmap.png");
    defaultimp.layerList[1].textureNames.push_back("neutral_normals.png");

    defaultimp.layerList[2].worldSize = 1;
    defaultimp.layerList[2].textureNames.push_back("CliffRock.JPG");
    defaultimp.layerList[2].textureNames.push_back("CliffRock_N.jpg");

    defaultimp.layerList[3].worldSize = 3;
    defaultimp.layerList[3].textureNames.push_back("CliffRock.JPG");
    defaultimp.layerList[3].textureNames.push_back("CliffRock_N.jpg");
//     defaultimp.layerList[2].worldSize = 1;
//     defaultimp.layerList[2].textureNames.push_back("tileable-IMG_0062-lush.png");
//     defaultimp.layerList[3].worldSize = 0.3;
//     defaultimp.layerList[3].textureNames.push_back("tileable-TT7002066-grey.png");
//     defaultimp.layerList[1].textureNames.push_back("checkers.png");

    Ogre::Image heightmap;
    heightmap.load("heightmap.png", "");

//     m_group->defineTerrain(0, 0, 0.0f);
//     m_group->loadAllTerrains(true);
//     auto ter = m_group->getTerrain(0, 0);
//     ter->dirty();
//     m_group->update();


//     OgreBase::getSingletonPtr()->getBrush()->setTerrainGroup(m_group);

//     Ogre::TextureManager::getSingleton().create("checkers.png", "layers");
    
//     m_terrain = new Ogre::Terrain(m_sceneManager);
//     m_terrain->setResourceGroup("layers");
//     m_terrain->setSize(513);
//     m_terrain->setWorldSize(m_terrain->getSize()*4);
//     Ogre::StringVector textures;
//     textures.push_back("checkers.png");
//     m_terrain->addLayer(256, &textures);
//     m_root->renderOneFrame();
//     this->show();
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

