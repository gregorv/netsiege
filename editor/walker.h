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

#ifndef WALKER_H
#define WALKER_H

#include <deque>
#include <OgreVector3.h>
#include <OgreQuaternion.h>

namespace Ogre {
    class SceneNode;
    class Entity;
    class AnimationState;
}

class Walker
{
public:
    enum animation_t {
        A_WALK,
        A_IDLE,
        A_FALL
    };
    Walker(const std::string& name);
    virtual ~Walker();

    virtual void setPosition(const Ogre::Vector3& position);
    virtual void setOrientation(const Ogre::Quaternion& orientation);
    virtual void start();
    virtual void stop();
    virtual void addPoint(const Ogre::Vector3& point);
    virtual void setPointList(const std::deque<Ogre::Vector3>& pointList);
    virtual void update(float dt);
    virtual Ogre::Vector3 getPosition() const;

protected:
    virtual void setAnimation(animation_t anim);
    virtual void setupBody();

    Ogre::SceneNode* m_node;
    Ogre::Entity* m_entity;
    Ogre::AnimationState* m_animationState;
    float m_speedFactor;
    float m_walkSpeed;
    float m_distToNext;

private:
    std::deque<Ogre::Vector3> m_pointList;
};

#endif // WALKER_H
