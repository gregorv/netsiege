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

#ifndef EDITORCAMERA_H
#define EDITORCAMERA_H

#include "walker.h"
#include "walkercamera.h"
#include <OgreVector2.h>
#include <OgreMath.h>
#include <QObject>

namespace Ogre {
    class Camera;
}
class QKeyEvent;
class QMouseEvent;

class EditorCamera : public QObject, protected Walker, protected WalkerCamera
{
    Q_OBJECT
public:
    typedef enum {
        D_NONE = 0,
        D_FORWARD=1,
        D_BACKWARD=2,
        D_LEFT=4,
        D_RIGHT=8,
        D_ROTATE=16,
        D_PAN=32
    } direction_t;

    typedef enum {
        R_NONE = 0,
        R_ROTATE=1,
    } rotation_t;

    EditorCamera(Ogre::SceneManager* scenemgr);

    void keyPress(QKeyEvent* ev);
    void keyRelease(QKeyEvent* ev);
    void mouseMove(Ogre::Vector2 relPos, Ogre::Vector2 delta);
    void mousePress(QMouseEvent* ev);
    void mouseRelease(QMouseEvent* ev);

    void setPitch(float pitch) { m_pitch = Ogre::Radian(pitch); }
    void setYaw(float yaw) { m_yaw = Ogre::Radian(yaw); }
    virtual void setOrientation(float pitch, float yaw) { setPitch(pitch); setYaw(yaw); }

    void addPitch(float pitch) { m_pitch += Ogre::Radian(pitch); }
    void addYaw(float yaw) { m_yaw += Ogre::Radian(yaw); }
    void addOrientation(float pitch, float yaw) { addPitch(pitch); addYaw(yaw); }
    bool isMouseInWindow() const;
    Ogre::Ray getPickRay() const;

    Ogre::Camera* getCamera() const { return WalkerCamera::getCamera(); }

    void setUpdateInterval(float dt) { m_dt = dt; }

    void setWalkerMode(bool enabled);

public slots:
    void setSpeed(double speed) { m_speed = speed; }
    virtual void update();

private:
    Ogre::Camera* m_camera;
    Ogre::Vector2 m_relMousePosition;
    Ogre::Vector2 m_mouseDelta;
    int m_directions;
    rotation_t m_rotation;
    bool m_speedBoost;
    float m_speed;
    Ogre::Radian m_pitch;
    Ogre::Radian m_yaw;
    float m_dt;
    bool m_walkerMode;
};

#endif // EDITORCAMERA_H
