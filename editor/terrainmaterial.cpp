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
    // define the layers
    // We expect terrain textures to have no alpha, so we use the alpha channel
    // in the albedo texture to store specular reflection
    // similarly we double-up the normal and height (for parallax)
    mLayerDecl.samplers.push_back(Ogre::TerrainLayerSampler("albedo_specular", Ogre::PF_BYTE_RGBA));
//     mLayerDecl.samplers.push_back(Ogre::TerrainLayerSampler("normal_height", Ogre::PF_BYTE_RGBA));

    mLayerDecl.elements.push_back(
            Ogre::TerrainLayerSamplerElement(0, Ogre::TLSS_ALBEDO, 0, 3));
    mLayerDecl.elements.push_back(
            Ogre::TerrainLayerSamplerElement(0, Ogre::TLSS_SPECULAR, 3, 1));
//     mLayerDecl.elements.push_back(
//             Ogre::TerrainLayerSamplerElement(1, Ogre::TLSS_NORMAL, 0, 3));
//     mLayerDecl.elements.push_back(
//             Ogre::TerrainLayerSamplerElement(1, Ogre::TLSS_HEIGHT, 3, 1));
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

    assert(mat.get());

    // Clone material
//     if(parent->mCloneMaterial) {
        mat = mat->clone(matName);
//         parent->mMaterialName = matName;
//     }

    // Add normalmap
//     if(parent->mAddNormalMap) {
        // Get default pass
        for(uint8_t i=0; i<mat->getTechnique(0)->getNumPasses(); i++) {
            Ogre::Pass *p = mat->getTechnique(0)->getPass(i);
            Ogre::GpuProgramParametersSharedPtr pParams = p->getFragmentProgramParameters();
            std::cout << "Num texture unit states " << p->getNumTextureUnitStates() << std::endl;
            assert(p->getNumTextureUnitStates() >= 4);

            /// TEXTURE UNIT 0 - NORMAL MAP
            // Add terrain's global normalmap to renderpass so the fragment program can find it.
            Ogre::TextureUnitState *tu = p->getTextureUnitState(0);

            Ogre::TexturePtr nmtx = terrain->getTerrainNormalMap();
            tu->_setTexturePtr(nmtx);


            /// TEXTURE UNIT 1 - BLEND LAYER 0
            tu = p->getTextureUnitState(1);
            tu->_setTexturePtr(terrain->getLayerBlendTexture(0));
            /// TEXTURE UNIT 2 - BLEND LAYER 1
            tu = p->getTextureUnitState(2);
            tu->_setTexturePtr(terrain->getLayerBlendTexture(1));
            /// TEXTURE UNIT 3+i - LAYER i
            for(uint8_t i=0; i<terrain->getLayerCount() && i<p->getNumTextureUnitStates()-3; i++) {
                auto layerTexture = terrain->getLayerTextureName(i, 0);
                std::cout << "Layer " << (int)i << ": " << layerTexture << std::endl;
                tu = p->getTextureUnitState(i+3);
                auto tex = Ogre::TextureManager::getSingleton().getByName(layerTexture, "");
                if(!tex.getPointer()) {
                    tex = Ogre::TextureManager::getSingleton().load(layerTexture, "");
                }
                assert(tex.getPointer() != nullptr);
                tu->_setTexturePtr(tex);
                pParams->setNamedConstant(Ogre::String("texScale")+Ogre::StringConverter::toString(i),
                                        1.0f/terrain->getLayerWorldSize(i));
            }
        }
//     }
    assert(mat.get());
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
