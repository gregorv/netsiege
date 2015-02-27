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

#include "qogrewidget.hpp"
#include <QResizeEvent>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <string>
#include <cassert>

#ifdef __unix__
    #include <QtX11Extras/QX11Info>
#endif


QOgreWidget::QOgreWidget(Ogre::Root* ogreRoot, IOgreEventHandler *ogreEventHandler, QWidget* parent) : 
  QGLWidget(parent),
  mOgreRoot(ogreRoot),
  mOgreRenderWindow(NULL),
  mOgreEventHandler(ogreEventHandler)
{
    if (!parent) {
        throw std::runtime_error("Parent widget supplied was uninitialised!"); // interface requirement
    }

    setAttribute(Qt::WA_PaintOnScreen, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setFocusPolicy(Qt::StrongFocus);

    Ogre::String winHandle;
    #ifdef WIN32
        // Windows code
        winHandle += Ogre::StringConverter::toString(this->parentWidget()->winId());
    #else
        winHandle = Ogre::StringConverter::toString((unsigned long)(QX11Info::display()));
        winHandle += ":";
        winHandle += Ogre::StringConverter::toString((unsigned int)(QX11Info::appScreen()));
        winHandle += ":";
        winHandle += Ogre::StringConverter::toString((unsigned long)(winId()));
    #endif

    Ogre::NameValuePairList params;
    params["parentWindowHandle"] = winHandle;
    params["FSAA"] = Ogre::String("8");

    int w = width();
    int h = height();
    mOgreRenderWindow = mOgreRoot->createRenderWindow("OgreWidget_RenderWindow", qMax(w, 640), qMax(h, 480), false, &params);
    mOgreRenderWindow->setActive(true);
    mOgreRenderWindow->setVisible(true);

    WId ogreWinId = 0x0;
    mOgreRenderWindow->getCustomAttribute("WINDOW", &ogreWinId);
    assert(ogreWinId); // guaranteed to be valid due to the way it was created
    QWidget::create(ogreWinId);
    setAttribute(Qt::WA_OpaquePaintEvent);   // qt won't bother updating the area under/behind the render window
    setMouseTracking(true);
}


Ogre::RenderWindow *QOgreWidget::getEmbeddedOgreWindow() {
    assert(mOgreRenderWindow); // guaranteed to be valid after construction
    return mOgreRenderWindow;
}


void QOgreWidget::paintEvent(QPaintEvent *pEvent) {
    this->update();
}


void QOgreWidget::resizeEvent(QResizeEvent *rEvent) {
    if (rEvent) {
        QWidget::resizeEvent(rEvent);
    }

    if (mOgreRenderWindow) {
        // since the underlying widget has already been updated we can source the resize values from there
        mOgreRenderWindow->reposition(x(), y());
        mOgreRenderWindow->resize(width(), height());
        mOgreRenderWindow->windowMovedOrResized();
    }
}


void QOgreWidget::mousePressEvent(QMouseEvent *event) {
    if(mOgreEventHandler) {
        mOgreEventHandler->ogreMousePressEvent(event);
    }
}

void QOgreWidget::mouseReleaseEvent(QMouseEvent *event) {
    if(mOgreEventHandler) {
        mOgreEventHandler->ogreMouseReleaseEvent(event);
    }
}


void QOgreWidget::mouseMoveEvent(QMouseEvent *event) {
    if(mOgreEventHandler) {
        mOgreEventHandler->ogreMouseMoveEvent(event);
    }
}

void QOgreWidget::keyPressEvent(QKeyEvent* event)
{
    if(mOgreEventHandler && !event->isAutoRepeat()) {
        mOgreEventHandler->ogreKeyPressEvent(event);
    }
}

void QOgreWidget::keyReleaseEvent(QKeyEvent* event)
{
    if(mOgreEventHandler && !event->isAutoRepeat()) {
        mOgreEventHandler->ogreKeyReleaseEvent(event);
    }
}


void QOgreWidget::update() {
    QWidget::update();
    mOgreRoot->renderOneFrame();
}
