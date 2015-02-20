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

#ifndef QTOGRE_QOGREWIDGET_H
#define QTOGRE_QOGREWIDGET_H

#include <QGLWidget>
#include "iogreeventhandler.hpp"

namespace Ogre {
    class Root;
    class RenderWindow;
};

//*********************************************************************************************************************
/** Qt widget that wraps an embedded ogre render window
 *
 * This classes only responsibility is to initialise the ogre render window. The Ogre::Root object
 * to be used must be supplied by the user and once initialised the intended usage is to extract the
 * ogre render window and manipulate it directly to create the desired scene. The underlying widget
 * takes care of rendering and resizing when necessary.
 */
class QOgreWidget : public QGLWidget {
    Q_OBJECT

public: // interface
    /** Constructor
     * @param parent is required because ogre needs to set it as it's parent during initialisation
     * @throws if parent is NULL
     * @post Ogre3D renderwindow has been created
     */
    QOgreWidget(Ogre::Root *ogreRoot, IOgreEventHandler *ogreEventHandler, QWidget *parent);

    Ogre::RenderWindow *getEmbeddedOgreWindow();
    void setEventHandler(IOgreEventHandler* ogreEventHandler) { mOgreEventHandler = ogreEventHandler; }

protected: // interface
    /** @name Inherited from QWidget
     */
    //@{
    virtual void paintEvent(QPaintEvent *pEvent);
    virtual void resizeEvent(QResizeEvent *rEvent);
    virtual void update();
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
    //@}

private:
    Ogre::Root *mOgreRoot;
    Ogre::RenderWindow *mOgreRenderWindow;
    IOgreEventHandler *mOgreEventHandler;

}; // class

#endif // QTOGRE_QOGREWIDGET_H
