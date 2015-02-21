/*
 * netsiege - A multiplayer point and click adventure
 * Copyright (C) 2014 Gregor Vollmer <gregor@celement.de>
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

#ifndef CAMPAIGN_SCRIPTFILEMANAGER_H
#define CAMPAIGN_SCRIPTFILEMANAGER_H

#include <OgreResourceManager.h>
#include "scriptfile.h"

namespace script {

class ScriptFileManager : public Ogre::ResourceManager, public Ogre::Singleton<ScriptFileManager>
{
 protected:

     // must implement this from ResourceManager's interface
     Ogre::Resource *createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
         const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader,
         const Ogre::NameValuePairList *createParams);

 public:

     ScriptFileManager();
     virtual ~ScriptFileManager();

     virtual ScriptFilePtr load(const Ogre::String &name, const Ogre::String &group);

     static ScriptFileManager &getSingleton();
     static ScriptFileManager *getSingletonPtr();
};
}

#endif // CAMPAIGN_SCRIPTFILEMANAGER_H
