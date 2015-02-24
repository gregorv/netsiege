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

#ifndef TERRAINMATERIAL_H
#define TERRAINMATERIAL_H

#include <OgreTerrainMaterialGenerator.h>

class TerrainMaterial : public Ogre::TerrainMaterialGenerator
{
public:

    TerrainMaterial(Ogre::String materialName, bool addNormalmap=true, bool cloneMaterial=true);

    void setMaterialByName(const Ogre::String materialName);
    void addNormalMapOnGenerate(bool set) { mAddNormalMap=set; };
    void cloneMaterialOnGenerate(bool set) { mCloneMaterial=set; };

    Ogre::String getMaterialName() const { return mMaterialName; };

    virtual Ogre::MaterialPtr generate(const Ogre::Terrain* terrain);

    class Profile : public Ogre::TerrainMaterialGenerator::Profile
    {
    public:
        Profile(Ogre::TerrainMaterialGenerator* parent, const Ogre::String& name, const Ogre::String& desc);
        ~Profile();

        bool isVertexCompressionSupported() const { return false; }

        Ogre::MaterialPtr generate(const Ogre::Terrain* terrain);

        Ogre::MaterialPtr generateForCompositeMap(const Ogre::Terrain* terrain);

        Ogre::uint8 getMaxLayers(const Ogre::Terrain* terrain) const;

        void updateParams(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain);

        void updateParamsForCompositeMap(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain);

        void requestOptions(Ogre::Terrain* terrain);

        void setLightmapEnabled(bool enabled) { }

    };
protected:
    Ogre::String mMaterialName;
    bool mCloneMaterial;
    bool mAddNormalMap;
};

#endif // TERRAINMATERIAL_H
