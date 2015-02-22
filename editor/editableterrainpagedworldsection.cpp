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

#include "editableterrainpagedworldsection.h"
#include "campaignmanager.h"
#include <boost/filesystem.hpp>
#include <QApplication>

EditableTerrainPagedWorldSection::EditableTerrainPagedWorldSection(const Ogre::String& name, Ogre::PagedWorld* parent, Ogre::SceneManager* sm)
: Ogre::TerrainPagedWorldSection(name, parent, sm)
{

}

EditableTerrainPagedWorldSection::~EditableTerrainPagedWorldSection()
{

}

// void EditableTerrainPagedWorldSection::loadPage(Ogre::PageID pageID, bool forceSynchronous)
// {
//
// //     std::cout << "LOAD PAGE " << x << " " << y << std::endl;
//     Ogre::TerrainPagedWorldSection::loadPage(pageID, forceSynchronous);
// }

void EditableTerrainPagedWorldSection::unloadPage(Ogre::PageID pageID, bool forceSynchronous)
{
    long x,y;
    mTerrainGroup->unpackIndex(pageID, &x, &y);
    auto terrain = mTerrainGroup->getTerrain(x, y);
    if(terrain) {
        auto campaign = CampaignManager::getOpenDocument();
        if(campaign->isTileModified(x, y) && !campaign->isTileUnpaged(x, y)) {
            // do not unload a page while data is updated,
            // because saving modifications would block the
            // main loop
            if(terrain->isDerivedDataUpdateInProgress()) {
                return;
            }
            campaign->setTileUnpaged(x, y);
            std::string tmpfile(std::string("~")+mTerrainGroup->generateFilename(x, y));
                std::cout << "Save modified terrain to tmp file" << std::endl;
            terrain->save(tmpfile);
        }
    }
//     std::cout << "UNLOAD PAGE " << x << " " << y << std::endl;
    Ogre::TerrainPagedWorldSection::unloadPage(pageID, forceSynchronous);
}

//---------------------------------------------------------------------
const Ogre::String EditableTerrainPagedWorldSection::SectionFactory::FACTORY_NAME("EditTerrain");

const Ogre::String& EditableTerrainPagedWorldSection::SectionFactory::getName() const
{
    return FACTORY_NAME;
}
//---------------------------------------------------------------------
Ogre::PagedWorldSection* EditableTerrainPagedWorldSection::SectionFactory::createInstance(const Ogre::String& name, Ogre::PagedWorld* parent, Ogre::SceneManager* sm)
{
    return OGRE_NEW EditableTerrainPagedWorldSection(name, parent, sm);
}
//---------------------------------------------------------------------
void EditableTerrainPagedWorldSection::SectionFactory::destroyInstance(PagedWorldSection* s)
{
    OGRE_DELETE s;
}
