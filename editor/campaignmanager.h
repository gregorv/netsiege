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

#ifndef CAMPAIGNMANAGER_H
#define CAMPAIGNMANAGER_H

#include <string>
#include <memory>
#include <vector>
#include <QObject>
#include <OgreRectangle2D.h>
#include <boost/filesystem.hpp>
#include "undocommands.h"

namespace Ogre {
    class SceneManager;
    class TerrainGroup;
    class PagedWorld;
    class TerrainPagedWorldSection;
}
class QUndoStack;

class CampaignManager : public QObject
{
    Q_OBJECT
public:
    friend undo::ModifyTerrainSlot;

    struct edited_tile_t {
        long x;
        long y;
        bool unpaged;
    };

    CampaignManager(const boost::filesystem::path& campaignPath);
    ~CampaignManager();

    void initialize(uint16_t terrainSize, float realWorldSize);
    void load();
    void save();

    void setTileModified(long x, long y);
    void setTileUnpaged(long x, long y);
    void clearTileModified(long x, long y);
    void clearAllTileModified();
    bool isTileModified(long x, long y) const;
    bool isTileUnpaged(long x, long y) const;
    const std::vector<edited_tile_t>& getTileModified() const;

    static CampaignManager* getOpenDocument() { return s_openDocument; }

    bool isSlotDefined(long x, long y) const;
    std::vector<uint32_t> getDefinedSlots() const { return m_definedTerrainSlots; }
    Ogre::Rect getSlotBoundary() const;
    void unpackSlotIndex(uint32_t key, long* x, long* y) const;

    boost::filesystem::path getTileFilepath(long x, long y) const;
    boost::filesystem::path getTmpTileFilepath(long x, long y) const;
    boost::filesystem::path getWorldSceneFilepath() const;
    Ogre::TerrainGroup* getTerrainGroup() const { return m_group.get(); }

signals:
    void initProgress(std::string, int, bool disable);
    void stepProgress();
    void doneProgress();

private:
    static CampaignManager* s_openDocument;

    void updateSlotBoundary();

    std::vector<edited_tile_t> m_editedTiles;
    boost::filesystem::path m_campaignPath;
    Ogre::SceneManager* m_sceneManager;
    std::unique_ptr<Ogre::TerrainGroup> m_group;
    std::unique_ptr<Ogre::PagedWorld> m_pagedWorld;
    std::unique_ptr<Ogre::TerrainPagedWorldSection> m_pagedTerrain;
    Ogre::Rect m_slotBoundary;
    std::vector<uint32_t> m_definedTerrainSlots;
};

#endif // CAMPAIGNMANAGER_H
