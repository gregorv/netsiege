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

#include "terrainmaterial.h"

#include <OgreTerrain.h>
#include <iostream>

TerrainMaterial::TerrainMaterial(Ogre::String materialName, bool addNormalmap, bool cloneMaterial)
    : mMaterialName(materialName), mAddNormalMap(addNormalmap), mCloneMaterial(cloneMaterial)
{
    std::cout << "+-+-+-+-+ TerrainMaterial" << std::endl;
    mProfiles.push_back(OGRE_NEW TerrainMaterial::Profile(this, "OgreMaterial", "Profile for rendering Ogre standard material"));
    setActiveProfile("OgreMaterial");
}

void TerrainMaterial::setMaterialByName(const Ogre::String materialName) {
    mMaterialName = materialName;
    _markChanged();
};

Ogre::MaterialPtr TerrainMaterial::generate(const Ogre::Terrain* terrain)
{
    std::cout << "+-+-+-+-+ GENERATE MATERIAL" << std::endl;
    return Ogre::TerrainMaterialGenerator::generate(terrain);
}

// -----------------------------------------------------------------------------------------------------------------------

TerrainMaterial::Profile::Profile(Ogre::TerrainMaterialGenerator* parent, const Ogre::String& name, const Ogre::String& desc)
    : Ogre::TerrainMaterialGenerator::Profile(parent, name, desc)
{
    std::cout << "+-+-+-+-+ SURPRISE" << std::endl;
};

TerrainMaterial::Profile::~Profile()
{
};

Ogre::MaterialPtr TerrainMaterial::Profile::generate(const Ogre::Terrain* terrain)
{
    std::cout << "-+-+- GENERATE" << std::endl;
    const Ogre::String& matName = terrain->getMaterialName();

    Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(matName);
    if (!mat.isNull())
        Ogre::MaterialManager::getSingleton().remove(matName);

    TerrainMaterial* parent = (TerrainMaterial*)getParent();

    // Set Ogre material
    mat = Ogre::MaterialManager::getSingleton().getByName(parent->mMaterialName);

    // Clone material
    if(parent->mCloneMaterial) {
        mat = mat->clone(matName);
        parent->mMaterialName = matName;
    }

    // Add normalmap
    if(parent->mAddNormalMap) {
        // Get default pass
        Ogre::Pass *p = mat->getTechnique(0)->getPass(0);

        // Add terrain's global normalmap to renderpass so the fragment program can find it.
        Ogre::TextureUnitState *tu = p->createTextureUnitState(matName+"/nm");

        Ogre::TexturePtr nmtx = terrain->getTerrainNormalMap();
        tu->_setTexturePtr(nmtx);
    }

    return mat;
};

Ogre::MaterialPtr TerrainMaterial::Profile::generateForCompositeMap(const Ogre::Terrain* terrain)
{
    return terrain->_getCompositeMapMaterial();
};

Ogre::uint8 TerrainMaterial::Profile::getMaxLayers(const Ogre::Terrain* terrain) const
{
    return 0;
};

void TerrainMaterial::Profile::updateParams(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain)
{
};

void TerrainMaterial::Profile::updateParamsForCompositeMap(const Ogre::MaterialPtr& mat, const Ogre::Terrain* terrain)
{
};

void TerrainMaterial::Profile::requestOptions(Ogre::Terrain* terrain)
{
    terrain->_setMorphRequired(false);
    terrain->_setNormalMapRequired(true); // enable global normal map
    terrain->_setLightMapRequired(false);
    terrain->_setCompositeMapRequired(false);
};