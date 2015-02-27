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

#ifndef WALKERCAMERA_H
#define WALKERCAMERA_H

namespace Ogre {
    class Camera;
    class TerrainGroup;
    class SceneManager;
}
class Walker;

class WalkerCamera
{
public:
    WalkerCamera(Ogre::SceneManager* sceneMgr, Ogre::Camera* camera, Walker* walker);
    virtual ~WalkerCamera();

    void setWalker(Walker* walker) { m_walker = walker; }

    void setOrientation(float pitch, float yaw);
    void addOrientation(float dpitch, float dyaw);
    void setDistance(float distance);
    void addDistance(float distance);

    void setPitchLimits(float minPitch, float maxPitch);
    void setDistanceLimits(float minDist, float maxDist);

    void update(float dt, Ogre::TerrainGroup* group = nullptr);
    
    Ogre::Camera* getCamera() const { return m_camera; }

private:
    Ogre::Camera* m_camera;
    bool m_externalCamera;
    float m_maxPitch;
    float m_minPitch;
    float m_maxDist;
    float m_minDist;
    float m_dist;
    float m_pitch;
    float m_yaw;
    Walker* m_walker;
};

#endif // WALKERCAMERA_H
