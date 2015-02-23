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
    struct edited_tile_t {
        long x;
        long y;
        bool unpaged;
    };

    CampaignManager(const std::string& campaignPath);
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

signals:
    void initProgress(std::string, int, bool disable);
    void stepProgress();
    void doneProgress();

private:
    static CampaignManager* s_openDocument;

    std::vector<edited_tile_t> m_editedTiles;
    std::string m_campaignPath;
    Ogre::SceneManager* m_sceneManager;
    std::unique_ptr<Ogre::TerrainGroup> m_group;
    std::unique_ptr<Ogre::PagedWorld> m_pagedWorld;
    std::unique_ptr<Ogre::TerrainPagedWorldSection> m_pagedTerrain;
    Ogre::Rect m_terrainBoundary;
    std::vector<uint32_t> m_definedTerrainSlots;
};

#endif // CAMPAIGNMANAGER_H
