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

#include "scriptfilemanager.h"
#include <cassert>

using namespace script;

template<> ScriptFileManager *Ogre::Singleton<ScriptFileManager>::msSingleton = 0;

 ScriptFileManager *ScriptFileManager::getSingletonPtr()
 {
     return msSingleton;
 }

 ScriptFileManager &ScriptFileManager::getSingleton()
 {
     assert(msSingleton);
     return(*msSingleton);
 }

 ScriptFileManager::ScriptFileManager()
 {
     mResourceType = "ScriptFile";

     // low, because it will likely reference other resources
     mLoadOrder = 30.0f;

     // this is how we register the ResourceManager with OGRE
     Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
 }

 ScriptFileManager::~ScriptFileManager()
 {
     // and this is how we unregister it
     Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
 }

 ScriptFilePtr ScriptFileManager::load(const Ogre::String& name, const Ogre::String& group)
 {
     auto script = getResourceByName(name);

     if (script.isNull())
         script = createResource(name, group);

     script->load();
     return  script.dynamicCast<ScriptFile>();
 }

 Ogre::Resource *ScriptFileManager::createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
                                             const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader,
                                             const Ogre::NameValuePairList *createParams)
 {
     return new ScriptFile(this, name, handle, group, isManual, loader);
 }
