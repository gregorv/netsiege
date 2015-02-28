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

#include "editorcamera.h"
#include "campaignmanager.h"
#include <OgreCamera.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreTerrainGroup.h>
#include <QKeyEvent>
#include <QMouseEvent>

EditorCamera::EditorCamera(Ogre::SceneManager* scenemgr)
 : QObject(nullptr), Walker("walkerCam"),
 WalkerCamera(scenemgr, nullptr, this),
 m_camera(WalkerCamera::getCamera()),
 m_relMousePosition(0.0f, 0.0f),
 m_mouseDelta(0.0f, 0.0f),
 m_directions(EditorCamera::D_NONE),
 m_rotation(EditorCamera::R_NONE),
 m_speedBoost(false),
 m_speed(1.0f),
 m_pitch(0.0f),
 m_yaw(0.0f),
 m_walkerMode(false)
{
    m_camera->setNearClipDistance(0.01f);
    m_camera->lookAt(0.0, 0.0, 0.0);
    m_camera->setAutoAspectRatio(true);
    setWalkerMode(false);
    m_speedFactor = 5.0f;
}

void EditorCamera::keyPress(QKeyEvent* ev)
{
    if(ev->key() == Qt::Key_W) {
        m_directions |= EditorCamera::D_FORWARD;
    } else if(ev->key() == Qt::Key_S) {
        m_directions |= EditorCamera::D_BACKWARD;
    } else if(ev->key() == Qt::Key_A) {
        m_directions |= EditorCamera::D_LEFT;
    } else if(ev->key() == Qt::Key_D) {
        m_directions |= EditorCamera::D_RIGHT;
    } else if(ev->key() == Qt::Key_Shift) {
        m_speedBoost = true;
    }
    if(m_walkerMode) {
        if(ev->key() == Qt::Key_Up) {
            WalkerCamera::addDistance(4);
        } else if(ev->key() ==  Qt::Key_Down) {
            WalkerCamera::addDistance(-4);
        }
    }
}

void EditorCamera::keyRelease(QKeyEvent* ev)
{
    if(ev->key() == Qt::Key_W) {
        m_directions &= ~EditorCamera::D_FORWARD;
    } else if(ev->key() == Qt::Key_S) {
        m_directions &= ~EditorCamera::D_BACKWARD;
    } else if(ev->key() == Qt::Key_A) {
        m_directions &= ~EditorCamera::D_LEFT;
    } else if(ev->key() == Qt::Key_D) {
        m_directions &= ~EditorCamera::D_RIGHT;
    } else if(ev->key() == Qt::Key_Shift) {
        m_speedBoost = false;
    }
}

void EditorCamera::mouseMove(Ogre::Vector2 relPos, Ogre::Vector2 delta)
{
    m_relMousePosition = relPos;
    m_mouseDelta = delta/100.0f;
}

void EditorCamera::mousePress(QMouseEvent* ev)
{
    if(ev->button() == Qt::RightButton) {
        m_rotation = EditorCamera::R_ROTATE;
    } else if(ev->button() == Qt::MiddleButton) {
        m_directions |= EditorCamera::D_PAN;
    }
    if(m_walkerMode && ev->button() == Qt::LeftButton && CampaignManager::getOpenDocument()) {
        auto result = CampaignManager::getOpenDocument()->getTerrainGroup()->rayIntersects(getPickRay());
        if(result.hit) {
            stop();
            addPoint(result.position);
            start();
        }
    }
}

void EditorCamera::mouseRelease(QMouseEvent* ev)
{
    if(ev->button() == Qt::RightButton) {
        m_rotation = EditorCamera::R_NONE;
    } else if(ev->button() == Qt::MiddleButton) {
        m_directions &= ~EditorCamera::D_PAN;
    }
}

void EditorCamera::update(float dt)
{
    if(m_rotation == R_ROTATE) {
        m_yaw -= Ogre::Radian(m_mouseDelta.x);
        m_pitch -= Ogre::Radian(m_mouseDelta.y);
        WalkerCamera::addOrientation(m_mouseDelta.y, m_mouseDelta.x);
    }
    if(m_walkerMode && CampaignManager::getOpenDocument()) {
        Walker::update(dt, CampaignManager::getOpenDocument()->getTerrainGroup());
        WalkerCamera::update(dt, CampaignManager::getOpenDocument()->getTerrainGroup());
//         auto camRot = Ogre::Quaternion(-m_yaw, Ogre::Vector3::UNIT_Y) * Ogre::Quaternion(m_pitch, Ogre::Vector3::UNIT_Z) * Ogre::Vector3::UNIT_X;
//         auto camPos = pos + camRot*20.0f;
//         m_camera->setPosition(camPos);
//         m_camera->lookAt(pos);
    } else {
        auto camRot = Ogre::Quaternion(m_yaw, Ogre::Vector3::UNIT_Y) * Ogre::Quaternion(m_pitch, Ogre::Vector3::UNIT_Z);
        auto fwd = camRot * Ogre::Vector3::UNIT_X;
        auto left = camRot * Ogre::Vector3::UNIT_Z;
        auto up = camRot * Ogre::Vector3::UNIT_Y;
        m_camera->setDirection(fwd);
        auto pos = m_camera->getPosition();
        float speed = m_speedBoost? m_speed*5 : m_speed;
        if(m_directions & EditorCamera::D_FORWARD) {
            pos += fwd*speed*dt;
        }
        if(m_directions & EditorCamera::D_BACKWARD) {
            pos -= fwd*speed*dt;
        }
        if(m_directions & EditorCamera::D_LEFT) {
            pos -= left*speed*dt;
        }
        if(m_directions & EditorCamera::D_RIGHT) {
            pos += left*speed*dt;
        }
        if(m_directions & EditorCamera::D_PAN) {
            if(m_speedBoost) {
                pos += (fwd*m_mouseDelta.y - left*m_mouseDelta.x)*m_speed/4;
            } else {
                pos += (-left*m_mouseDelta.x + up*m_mouseDelta.y)*m_speed/4;
            }
        }
        m_camera->setPosition(pos);
    }
    m_mouseDelta = Ogre::Vector2::ZERO;
}

bool EditorCamera::isMouseInWindow() const
{
    return ((m_relMousePosition.x >= 0.0 && m_relMousePosition.x <= 1.0) &&
            (m_relMousePosition.y >= 0.0 && m_relMousePosition.y <= 1.0));
}

Ogre::Ray EditorCamera::getPickRay() const
{
    if(!isMouseInWindow()) {
        return Ogre::Ray(Ogre::Vector3::ZERO, Ogre::Vector3::ZERO);
    }
    return m_camera->getCameraToViewportRay(m_relMousePosition.x, m_relMousePosition.y);
}

void EditorCamera::setWalkerMode(bool enabled)
{
    m_walkerMode = enabled;
    m_node->setVisible(m_walkerMode);
    if(!m_walkerMode) {
        stop();
    }
}
