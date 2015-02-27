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

#include "walker.h"
#include "ogrebase.h"
#include <OgreSceneManager.h>
#include <OgreEntity.h>

Walker::Walker(const std::string& name)
: m_node(nullptr), m_entity(nullptr), m_animationState(nullptr),
m_speedFactor(1.0f), m_walkSpeed(1.0f), m_distToNext(-1.0f)
{
    auto sceneMgr = OgreBase::getSingleton().getSceneManager();
    m_node = sceneMgr->getRootSceneNode()->createChildSceneNode(name, Ogre::Vector3::ZERO);
    setupBody();
    setAnimation(A_IDLE);
}

Walker::~Walker()
{
}

void Walker::start()
{
    if(m_distToNext <= 0.0f) {
        setAnimation(A_WALK);
        std::cout << "START " << m_pointList.front() << std::endl;
        m_distToNext = (m_pointList.front() - m_node->getPosition()).length();
    }
}

void Walker::stop()
{
    setAnimation(A_IDLE);
    m_pointList.clear();
    m_distToNext = -1.0f;
}

void Walker::setPosition(const Ogre::Vector3& position)
{
    stop();
    m_node->setPosition(position);
}

void Walker::setOrientation(const Ogre::Quaternion& orientation)
{
    stop();
    m_node->setOrientation(orientation);
}

void Walker::addPoint(const Ogre::Vector3& point)
{
    m_pointList.push_back(point);
    std::cout << "QUEUE ";
    for(auto& point: m_pointList) {
        std::cout << point << " ";
    }
    std::cout << std::endl;
}

void Walker::setPointList(const std::deque< Ogre::Vector3 >& pointList)
{
    m_pointList = pointList;
}

void Walker::update(float dt)
{
    if(m_animationState) {
        m_animationState->addTime(dt*m_speedFactor);
    }
    if(m_pointList.size() > 0 && m_distToNext >= 0.0f) {
        auto destination = m_pointList.front();
        auto dir = destination - m_node->getPosition();
        dir.normalise();
        m_node->translate(dir * m_walkSpeed * m_speedFactor * dt);
        m_distToNext -= m_walkSpeed * m_speedFactor * dt;
        if(m_distToNext <= 0.0f) {
            m_pointList.pop_front();
            if(m_pointList.size() > 0) {
                std::cout << "NEXT " << m_pointList.front() << std::endl;
                m_distToNext = (m_pointList.front() - m_node->getPosition()).length();
            } else {
                std::cout << "STOP" << std::endl;
                stop();
            }
        }
    }
}

void Walker::setupBody()
{
    auto sceneMgr = OgreBase::getSingleton().getSceneManager();
    m_entity = sceneMgr->createEntity("Sinbad", "robot.mesh");
    m_node->attachObject(m_entity);
    m_walkSpeed = 1.0f;
    m_node->setScale(0.02f, 0.02f, 0.02f);
}

void Walker::setAnimation(Walker::animation_t anim)
{
    if(m_animationState)
        m_animationState->setEnabled(false);
    if(anim == A_IDLE) {
        m_animationState = m_entity->getAnimationState("Idle");
        m_animationState->setLoop(true);
        m_animationState->setEnabled(true);
    } else if (anim == A_WALK) {
        m_animationState = m_entity->getAnimationState("Walk");
        m_animationState->setLoop(true);
        m_animationState->setEnabled(true);
    }
}

Ogre::Vector3 Walker::getPosition() const
{
    return m_node->getPosition();
}
