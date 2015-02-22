/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2015  Gregor Vollmer <gregor@celement.de>
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

#ifndef EDITABLETERRAINPAGEDWORLDSECTION_H
#define EDITABLETERRAINPAGEDWORLDSECTION_H

#include <OgreTerrainPagedWorldSection.h>

class EditableTerrainPagedWorldSection : public Ogre::TerrainPagedWorldSection
{
public:
    EditableTerrainPagedWorldSection(const Ogre::String& name, Ogre::PagedWorld* parent, Ogre::SceneManager* sm);
    virtual ~EditableTerrainPagedWorldSection();
//     virtual void loadPage(Ogre::PageID pageID, bool forceSynchronous);
    virtual void unloadPage(Ogre::PageID pageID, bool forceSynchronous);

    class SectionFactory : public Ogre::PagedWorldSectionFactory
    {
    public:
        static const Ogre::String FACTORY_NAME;
        const Ogre::String& getName() const;
        PagedWorldSection* createInstance(const Ogre::String& name, Ogre::PagedWorld* parent, Ogre::SceneManager* sm);
        void destroyInstance(PagedWorldSection*);

    };
};

#endif // EDITABLETERRAINPAGEDWORLDSECTION_H
