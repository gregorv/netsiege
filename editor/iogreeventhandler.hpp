/*
 * netsiege - A multiplayer point and click adventure
 * Copyright (C) 2014 Gernot Klingler https://github.com/gklingler/QOgreWidget
 * Many thanks to him!
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
#ifndef QOgreEventHandler_hpp__
#define QOgreEventHandler_hpp__

class QPaintEvent;
class QResizeEvent;
class QMouseEvent;
class QKeyEvent;

class IOgreEventHandler {
public:
    virtual void ogrePaintEvent(QPaintEvent *pEvent) = 0;
    virtual void ogreResizeEvent(QResizeEvent *rEvent) = 0;
    virtual void ogreUpdate() = 0;
    virtual void ogreMousePressEvent(QMouseEvent *event) = 0;
    virtual void ogreMouseReleaseEvent(QMouseEvent *event) = 0;
    virtual void ogreMouseMoveEvent(QMouseEvent *event) = 0;
    virtual void ogreKeyPressEvent(QKeyEvent *event) = 0;
    virtual void ogreKeyReleaseEvent(QKeyEvent *event) = 0;
};

#endif // QOgreEventHandler_hpp__
