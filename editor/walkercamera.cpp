/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2015  <copyright holder> <email>
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

#include "walkercamera.h"
#include "walker.h"

#include <cassert>
#include <OgreMath.h>
#include <OgreTerrainGroup.h>
#include <OgreCamera.h>

WalkerCamera::WalkerCamera(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera, Walker* walker)
:m_camera(camera), m_externalCamera(true),
m_maxPitch(3.141/2.0f), m_minPitch(3.141/5.0f),
m_maxDist(20.0f), m_minDist(4.0f),
m_dist(8.0f), m_pitch(m_minPitch), m_yaw(0.0f),
m_walker(walker)
{
    if(!m_camera) {
        m_externalCamera = false;
        m_camera = sceneMgr->createCamera("WalkerCamera");
    }
}

WalkerCamera::~WalkerCamera()
{

}

void WalkerCamera::setOrientation(float pitch, float yaw)
{
    m_pitch = Ogre::Math::Clamp<float>(pitch, m_minPitch, m_maxPitch);
    m_yaw = yaw;
}

void WalkerCamera::addOrientation(float dpitch, float dyaw)
{
    setOrientation(m_pitch+dpitch, m_yaw-dyaw);
}

void WalkerCamera::setDistance(float distance)
{
    m_dist = Ogre::Math::Clamp<float>(distance, m_minDist, m_maxDist);
}

void WalkerCamera::addDistance(float distance)
{
    setDistance(m_dist + distance);
}

void WalkerCamera::setPitchLimits(float minPitch, float maxPitch)
{
    m_minPitch = minPitch;
    m_maxPitch = maxPitch;
    setOrientation(m_pitch, m_yaw);
}

void WalkerCamera::setDistanceLimits(float minDist, float maxDist)
{
    m_minDist = minDist;
    m_maxDist = maxDist;
    setDistance(m_dist);
}

void WalkerCamera::update(float dt, Ogre::TerrainGroup* group)
{
    if(m_walker) {
        auto walkerPos = m_walker->getPosition();
        walkerPos.y += 1.0f;
        auto direction = Ogre::Quaternion(Ogre::Radian(m_yaw), Ogre::Vector3::UNIT_Y)
                       * Ogre::Quaternion(Ogre::Radian(m_pitch), Ogre::Vector3::UNIT_Z)
                       * Ogre::Vector3::UNIT_X;
        auto camPos = walkerPos + direction*m_dist;
        if(group) {
            camPos.y = std::max(group->getHeightAtWorldPosition(camPos)+2.0f, camPos.y);
        }
        m_camera->setPosition(camPos);
        m_camera->lookAt(walkerPos);
    }
}
