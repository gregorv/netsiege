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

#include "brush.h"
#include <cassert>
#include <QUndoStack>
#include <QApplication>
#include <Terrain/OgreTerrainGroup.h>
#include <OgreRoot.h>
#include <OgreSceneNode.h>
#include "undocommands.h"
#include "brushdrawer.h"

#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))
#define BRUSH(x) ((x>1.0f)?0.0f:(x*x*x*x - 2*x*x + 1))
float brushFunction(float x, float hard)
{
    if(x == 1.0f) {
        return x>1.0f? 0.0f : 1.0f;
    } else {
        if(x > hard) {
            return BRUSH((x-hard) / (1.0f - hard));
        } else {
            return 1.0f;
        }
    }
}

Brush::Brush(Ogre::TerrainGroup* group, Ogre::SceneManager* sceneMgr, QUndoStack* stack)
: QObject(),
 m_mode(Brush::M_NONE),
 m_stack(stack),
 m_drawer(new BrushDrawer("Brush", group)),
 m_group(group),
 m_currentCommand(nullptr)
{
    auto rootNode = sceneMgr->getRootSceneNode();
    rootNode->attachObject(m_drawer);
    settings_t def { 2.0f, 5.0f, 0.0f };
    m_settings[M_HEIGHT_FLATTEN] = def;
    m_settings[M_HEIGHT_SMOOTH] = def;
    m_settings[M_HEIGHT_RAISE] = def;
    m_settings[M_LAYER_PAINT] = def;
}

Brush::~Brush()
{
    if(m_currentCommand) {
        delete m_currentCommand;
    }
}


void Brush::setTerrainGroup(Ogre::TerrainGroup* group)
{
    stopStroke();
    m_group = group;
    m_drawer->setTerrainGroup(group);
}

void Brush::setMode(Brush::mode_t mode)
{
    if(!m_group) return;
    stopStroke();
    m_mode = mode;
    setSize(m_settings[mode].size);
    setStrength(m_settings[mode].strength);
    setHardness(m_settings[mode].hardness);
    m_drawer->showHeightMarker(m_mode == M_HEIGHT_FLATTEN);
}

void Brush::startStroke()
{
    if(!m_group) return;
    stopStroke();
    assert(m_currentCommand == nullptr);
    if(m_mode & M_HEIGHT) {
        m_currentCommand = new undo::HeightmapEdit(m_group);
    } else if(m_mode == M_LAYER_PAINT) {
        if(m_blendLayer > 0) {
            m_currentCommand = new undo::BlendmapEdit(m_group, m_blendLayer);
        }
    }
}

void Brush::update(const Ogre::Ray& pickray)
{
    if(!m_currentCommand || !m_group)
        return;
    auto result = m_group->rayIntersects(pickray);
    pointerUpdate(result.hit, result.position);
    if(result.hit) {
        auto ter = result.terrain;
        auto indices = (result.position - ter->getPosition())/ter->getWorldSize();
        indices += 0.5;
        if(m_mode & M_HEIGHT) {
            auto cmd = dynamic_cast<undo::HeightmapEdit*>(m_currentCommand);
            assert(cmd != nullptr);
            cmd->monitorTerrain(0, 0);
            indices.z = 1.0f - indices.z;
            indices *= ter->getSize();
//             std::cout << m_settings[m_mode].size << " " << m_settings[m_mode].strength << " " << m_settings[m_mode].hardness << std::endl;
            float* heightdata = ter->getHeightData();
            float brushSize = m_settings[m_mode].size/ter->getWorldSize() * ter->getSize();
            size_t terSize = ter->getSize();
//             std::cout << "indices " << indices << ", brush size" << brushSize << std::endl;
            if(m_mode == M_HEIGHT_RAISE) {
                float strength = m_settings[m_mode].strength *
                              ((QApplication::queryKeyboardModifiers()
                              & Qt::ShiftModifier)? -0.1f : 0.1f);
                for(int ix=MAX(indices.x-brushSize/2, 0); ix<MIN(indices.x+brushSize/2, terSize); ix++) {
                    for(int iy=MAX(indices.z-brushSize/2, 0); iy<MIN(indices.z+brushSize/2, terSize); iy++) {
                        float dist = std::sqrt(std::pow(ix-indices.x, 2) + std::pow(iy-indices.z, 2));
                        heightdata[iy*terSize+ix] += brushFunction(dist/brushSize*2,
                                                                        m_settings[m_mode].hardness) * strength;
                    }
                }
            } else if(m_mode == M_HEIGHT_SMOOTH) {
                float avgHeight = 0.0;
                float area = 0.0f;
                for(int ix=MAX(indices.x-brushSize/2, 0); ix<MIN(indices.x+brushSize/2, terSize); ix++) {
                    for(int iy=MAX(indices.z-brushSize/2, 0); iy<MIN(indices.z+brushSize/2, terSize); iy++) {
                        float dist = std::sqrt(std::pow(ix-indices.x, 2) + std::pow(iy-indices.z, 2));
                        float weight = brushFunction(dist/brushSize*2, m_settings[m_mode].hardness);
                        area += weight;
                        avgHeight += heightdata[iy*terSize+ix] * weight;
                    }
                }
                avgHeight /= area;
                for(int ix=MAX(indices.x-brushSize/2, 0); ix<MIN(indices.x+brushSize/2, terSize); ix++) {
                    for(int iy=MAX(indices.z-brushSize/2, 0); iy<MIN(indices.z+brushSize/2, terSize); iy++) {
                        float dist = std::sqrt(std::pow(ix-indices.x, 2) + std::pow(iy-indices.z, 2));
                        float weight = brushFunction(dist/brushSize*2, m_settings[m_mode].hardness);
                        float difference = avgHeight - heightdata[iy*terSize+ix];
                        difference *= m_settings[m_mode].strength/10.0f;
                        heightdata[iy*terSize+ix] += weight*difference;
                    }
                }
            } else if(m_mode == M_HEIGHT_FLATTEN) {
                if(QApplication::queryKeyboardModifiers() & Qt::ShiftModifier) {
                    m_height = result.position.y;
                    m_drawer->setBrushOptions(m_settings[m_mode].size,
                                              m_settings[m_mode].hardness,
                                              m_height);
                } else {
                    for(int ix=MAX(indices.x-brushSize/2, 0); ix<MIN(indices.x+brushSize/2, terSize); ix++) {
                        for(int iy=MAX(indices.z-brushSize/2, 0); iy<MIN(indices.z+brushSize/2, terSize); iy++) {
                            float dist = std::sqrt(std::pow(ix-indices.x, 2) + std::pow(iy-indices.z, 2));
                            float weight = brushFunction(dist/brushSize*2, m_settings[m_mode].hardness);
                            float difference = m_height - heightdata[iy*terSize+ix];
                            difference *= m_settings[m_mode].strength/10.0f;
                            heightdata[iy*terSize+ix] += weight*difference;
                        }
                    }
                }
            }
            ter->dirty();
            ter->updateGeometry();
        } else if(m_mode == M_LAYER_PAINT) {
            auto cmd = dynamic_cast<undo::BlendmapEdit*>(m_currentCommand);
            assert(cmd != nullptr);
            cmd->monitorTerrain(0, 0);
            size_t size = ter->getLayerBlendMapSize();
            indices *= size;
            float brushSize = m_settings[m_mode].size/ter->getWorldSize() * size;
            float strength = m_settings[m_mode].strength * ((QApplication::queryKeyboardModifiers() & Qt::ShiftModifier)? -0.01f : 0.01f);
            auto layer = result.terrain->getLayerBlendMap(cmd->getLayerIndex());
//             if(QApplication::queryKeyboardModifiers() & Qt::ControlModifier) {
//                 layer = result.terrain->getLayerBlendMap(2);
//             }
            auto data = layer->getBlendPointer();
//             std::cout << "indices " << indices << ", brush size" << brushSize << " " << strength << " " << size << std::endl;
            for(int ix=MAX(indices.x-brushSize/2, 0); ix<MIN(indices.x+brushSize/2, size); ix++) {
                for(int iy=MAX(indices.z-brushSize/2, 0); iy<MIN(indices.z+brushSize/2, size); iy++) {
                    float dist = std::sqrt(std::pow(ix-indices.x, 2) + std::pow(iy-indices.z, 2));
                    float distWeight = brushFunction(dist/brushSize*2, m_settings[m_mode].hardness);
//                             std::cout << dist << " " << brushSize/2 << " " << distWeight << std::endl;
                    data[iy*size+ix] += distWeight*strength;
//                             data[iy*size+ix] -= 0.1;
                    if(data[iy*size+ix] > 1.0)
                        data[iy*size+ix] = 1.0;
                    if(data[iy*size+ix] < 0.0)
                        data[iy*size+ix] = 0.0;
//                             std::cout << data[iy*size+ix] << " ";
                }
            }
            layer->dirty();
            layer->update();
        }
    }
}

void Brush::pointerUpdate(const Ogre::Ray& pickray)
{
    if(!m_group) return;
    auto result = m_group->rayIntersects(pickray);
    pointerUpdate(result.hit, result.position);
}

void Brush::pointerUpdate(bool hit, const Ogre::Vector3 position)
{
    std::cout << hit << " " << position << std::endl;
    if(hit) {
        m_drawer->setPosition(position);
        m_drawer->setVisible(true);
    } else {
        m_drawer->setVisible(false);
    }
}


void Brush::stopStroke()
{
    if(!m_group) return;
    if(!m_currentCommand)
        return;
    assert(m_stack);
    m_currentCommand->finalize();
    m_stack->push(m_currentCommand);
    m_currentCommand = nullptr;
}

void Brush::setSize(float x)
{
    m_settings[m_mode].size = x;
    emit sizeChanged(x);
    emit sizeChangedInt(static_cast<int>(x*10.0f));
    m_drawer->setBrushOptions(m_settings[m_mode].size, m_settings[m_mode].hardness, m_height);
}

void Brush::setSize(int x)
{
    setSize(static_cast<float>(x)/10.0f);
}

void Brush::setHardness(int x)
{
    setHardness(static_cast<float>(x)/100.0f);
}

void Brush::setHardness(float x)
{
    m_settings[m_mode].hardness = x;
    emit hardnessChanged(x);
    emit hardnessChangedInt(static_cast<int>(x*100.0f));
    m_drawer->setBrushOptions(m_settings[m_mode].size, m_settings[m_mode].hardness, m_height);
}

void Brush::setStrength(int x)
{
    setStrength(static_cast<float>(x)/10.0f);
}

void Brush::setStrength(float x)
{
    m_settings[m_mode].strength = x;
    emit strengthChanged(x);
    emit strengthChangedInt(static_cast<int>(x*10.0f));
    m_drawer->setBrushOptions(m_settings[m_mode].size, m_settings[m_mode].hardness, m_height);
}

void Brush::setBlendLayer(int layer)
{
    m_blendLayer = layer;
}


