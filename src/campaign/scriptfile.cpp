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

#include "scriptfile.h"
#include <cassert>
#include <OGRE/OgreResourceGroupManager.h>

using namespace campaign;

ScriptFile::ScriptFile(Ogre::ResourceManager* creator, const Ogre::String& name,
                       Ogre::ResourceHandle handle, const Ogre::String& group,
                       bool isManual, Ogre::ManualResourceLoader* loader)
 : Ogre::Resource(creator, name, handle, group, isManual, loader), m_code()
{
    createParamDictionary("ScriptFile");
}

ScriptFile::~ScriptFile()
{
    unload();
}

void ScriptFile::unloadImpl(void )
{
    m_code.clear();
}

void ScriptFile::loadImpl(void )
{
    auto stream = Ogre::ResourceGroupManager::getSingleton().openResource(mName, mGroup, true, this);
    m_code.resize(stream->size());
    auto bytesRead = stream->read(&m_code.front(), m_code.capacity());
    assert(bytesRead == m_code.capacity());
    assert(bytesRead == m_code.size());
}

size_t ScriptFile::calculateSize(void ) const
{
    return m_code.length();
}
