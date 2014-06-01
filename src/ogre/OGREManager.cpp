/*
 * netsiege - A multiplayer point and click adventure
 * Copyright (C) 2014 Alexander Kraus <alexander.kraus@student.kit.edu>
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

#include "OGREManager.h"

#include <iostream>
#include <fstream>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreRenderSystem.h>
#include <OGRE/OgreRenderQueueListener.h>

OGREManager::OGREManager()
{
  std::ifstream checkExistenceStream("plugins.cfg");
  if(checkExistenceStream.good()) checkExistenceStream.close();
  else 
  {
    std::cout << "plugins.cfg not found; creating new... ";
    std::ofstream o("plugins.cfg", std::ofstream::out);
    o << "PluginFolder=/usr/lib/OGRE\n"
      << "\n"
      << "Plugin=RenderSystem_GL\n"
      << "Plugin=Plugin_ParticleFX\n"
      << "Plugin=Plugin_BSPSceneManager\n"
      << "Plugin=Plugin_PCZSceneManager\n"
      << "Plugin=Plugin_OctreeZone\n"
      << "Plugin=Plugin_OctreeSceneManager\n";
    o.close();
    std::cout << "Done.\n";
  }
  m_root = new Ogre::Root("plugins.cfg");
  
  std::cout << "Available Render Systems:\n";
  Ogre::RenderSystemList renderSystems = m_root->getAvailableRenderers();
  Ogre::RenderSystemList::iterator i = renderSystems.begin();
  Ogre::RenderSystem *preferredRenderSystem = *i;
  Ogre::String preferredVideoMode, preferredDisplayFrequency, preferredFSAA, preferredfixedPipelineEnabled, preferredFullScreen, 
    preferredRTTMode, preferredVSync, preferredSRGBGammaConversion;
  while(i != renderSystems.end())
  {
    Ogre::RenderSystem *renderSystem = *i;
    std::cout << "Render System: " << renderSystem->getName().c_str() << std::endl;
    Ogre::StringVector videoModes = renderSystem->getConfigOptions()["Video Mode"].possibleValues;
    {
      Ogre::StringVector::iterator j = videoModes.begin();
      if(renderSystem == preferredRenderSystem) preferredVideoMode = *j;
      while(j != videoModes.end())
      {
	std::cout << " -> Supported Video mode: " << j->c_str() << std::endl;
	++j;
      }
    }
    Ogre::StringVector displayFrequencies = renderSystem->getConfigOptions()["Display Frequency"].possibleValues;
    {
      Ogre::StringVector::iterator j = displayFrequencies.begin();
      if(renderSystem == preferredRenderSystem) preferredDisplayFrequency = *j;
      while(j != displayFrequencies.end())
      {
	std::cout << " -> Supported Display Frequency: " << j->c_str() << std::endl;
	++j;
      }
    }
    Ogre::StringVector fSAA = renderSystem->getConfigOptions()["FSAA"].possibleValues;
    {
      Ogre::StringVector::iterator j = fSAA.begin();
      if(renderSystem == preferredRenderSystem) preferredFSAA = *j;
      while(j != fSAA.end())
      {
	std::cout << " -> Supported FSAA mode: " << j->c_str() << std::endl;
	++j;
      }
    }
    Ogre::StringVector fixedPipelineEnabled = renderSystem->getConfigOptions()["Fixed Pipeline Enabled"].possibleValues;
    {
      Ogre::StringVector::iterator j = fixedPipelineEnabled.begin();
      if(renderSystem == preferredRenderSystem) preferredfixedPipelineEnabled = *j;
      while(j != fixedPipelineEnabled.end())
      {
	std::cout << " -> Supported Fixed Pipeline mode: " << j->c_str() << std::endl;
	++j;
      }
    }
    Ogre::StringVector fullScreen = renderSystem->getConfigOptions()["Full Screen"].possibleValues;
    {
      Ogre::StringVector::iterator j = fullScreen.begin();
      if(renderSystem == preferredRenderSystem) preferredFullScreen = *j;
      while(j != fullScreen.end())
      {
	std::cout << " -> Supported Full Screen Setting: " << j->c_str() << std::endl;
	++j;
      }
    }
    Ogre::StringVector rTTMode = renderSystem->getConfigOptions()["RTT Preferred Mode"].possibleValues;
    {
      Ogre::StringVector::iterator j = rTTMode.begin();
      if(renderSystem == preferredRenderSystem) preferredRTTMode = *j;
      while(j != rTTMode.end())
      {
	std::cout << " -> Supported RTT mode: " << j->c_str() << std::endl;
	++j;
      }
    }
    Ogre::StringVector vSync = renderSystem->getConfigOptions()["VSync"].possibleValues;
    {
      Ogre::StringVector::iterator j = vSync.begin();
      if(renderSystem == preferredRenderSystem) preferredVSync = *j;
      while(j != vSync.end())
      {
	std::cout << " -> Supported VSync setting: " << j->c_str() << std::endl;
	++j;
      }
    }
    Ogre::StringVector sRGBGammaConversion = renderSystem->getConfigOptions()["sRGB Gamma Conversion"].possibleValues;
    {
      Ogre::StringVector::iterator j = sRGBGammaConversion.begin();
      if(renderSystem == preferredRenderSystem) preferredSRGBGammaConversion = *j;
      while(j != sRGBGammaConversion.end())
      {
	std::cout << " -> Supported sRGB Gamm Conversion setting: " << j->c_str() << std::endl;
	++j;
      }
    }
    ++i;
  }
  
  checkExistenceStream.clear();
  checkExistenceStream.open("ogre.cfg");
  if(checkExistenceStream.good()) checkExistenceStream.close();
  else
  {
    std::cout << "ogre.cfg not found; creating new...";
    std::ofstream o("ogre.cfg", std::ofstream::out);
    o << "Render System=" << preferredRenderSystem->getName().c_str() << "\n"
      << std::endl
      << "[" << preferredRenderSystem->getName().c_str() << "]\n"
      << "Display Frequency=" << preferredDisplayFrequency.c_str() << std::endl
      << "FSAA=" << preferredFSAA.c_str() << std::endl
      << "Fixed Pipeline Enabled=" << preferredfixedPipelineEnabled.c_str() << std::endl
      << "Full Screen=" << preferredFullScreen.c_str() << std::endl
      << "RTT Preferred Mode=" << preferredRTTMode.c_str() << std::endl
      << "VSync=" << preferredVSync.c_str() << std::endl
      << "Video Mode=" << preferredVideoMode.c_str() << std::endl
      << "sRGB Gamma Conversion=" << preferredSRGBGammaConversion << std::endl
      ;
    o.close();
    std::cout << " Done.\n";
  }
  
  m_root->restoreConfig();
  
  m_render_window = m_root->initialise(true, "NetSiege OGRE window");
  m_scene_manager = m_root->createSceneManager(Ogre::ST_GENERIC);
  
  
}

OGREManager::~OGREManager()
{

}
