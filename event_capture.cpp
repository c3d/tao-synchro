// *****************************************************************************
// event_capture.cpp                                               Tao3D project
// *****************************************************************************
//
// File description:
//
//     Event capture and replay management.
//            - EventCapture class
//            - EventClient class
//
//
//
//
//
//
// *****************************************************************************
// This software is licensed under the GNU General Public License v3
// (C) 2011-2012, Baptiste Soulisse <baptiste.soulisse@taodyne.com>
// (C) 2011-2012, Catherine Burvelle <catherine@taodyne.com>
// (C) 2011, Christophe de Dinechin <christophe@taodyne.com>
// (C) 2011, Jérôme Forissier <jerome@taodyne.com>
// (C) 2011-2012, Baptiste Soulisse <baptiste.soulisse@taodyne.com>
// *****************************************************************************
// This file is part of Tao3D
//
// Tao3D is free software: you can r redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tao3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Tao3D, in a file named COPYING.
// If not, see <https://www.gnu.org/licenses/>.
// *****************************************************************************

#include "event_capture.h"

#include "tao_control_event.h"

#include <QApplication>
#include <QStatusBar>
#include <QAction>
#include <QDialog>
#include <QColor>
#include <QFont>
#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>

synchroBasic * synchroBasic::base = NULL;
const Tao::ModuleApi * synchroBasic::tao;

synchroBasic::synchroBasic(TaoEventHandler * handler,
                           QGLWidget *glw)
    : tao_event_handler(handler), widget(glw),  win(NULL)
{
    base = this;
    if ( ! glw )
    {
        foreach (QWidget *w, QApplication::topLevelWidgets())
        {
            if ((win = dynamic_cast<QMainWindow*>(w)) != NULL)
            {
                QList<QGLWidget *> list =
                        win->findChildren<QGLWidget *>(QRegExp("[W|w]idget.*"));
                if (list.size() < 1) continue;

                widget = list.first();
                break;
            }
        }
    }
    else
        win = dynamic_cast<QMainWindow*>(widget->window());
}


void synchroBasic::unproject(int x, int y, int , int *ux, int *uy, int *uz)
{
    *uz = 0;
    *ux = x - widget->width() / 2;
    *uy = widget->height() / 2 - y;
}

void synchroBasic::project (int x, int y, int , int *px, int *py, int *pz)
{
    *pz = 0;
    *px = x + widget->width() / 2;
    *py = widget->height() / 2 - y;
}

EventCapture::EventCapture(TaoEventHandler *handler, QGLWidget *glw) :
// ----------------------------------------------------------------------------
//   Creates a listener either on the provided glw or on the main QGLWidget
// ----------------------------------------------------------------------------
    synchroBasic(handler, glw)
{
}


void EventCapture::startCapture()
// ----------------------------------------------------------------------------
//   Start recording a sequence of events
// ----------------------------------------------------------------------------
{
    if ( ! win || ! widget ) return ;
    win->statusBar()->showMessage("Start event capture.");

    // Says the handler capture is about to start
   if ( ! tao_event_handler->beforeStart()) return;

    winSize = widget->size();

    //connection
    foreach (QAction* act,  win->findChildren<QAction*>())
    {
        if (act->objectName().startsWith("toolbar:test"))
            continue;
         connect(act, SIGNAL(triggered(bool)), this, SLOT(recordAction(bool)));
    }
    oldMouseTracking = widget->hasMouseTracking();
    widget->setMouseTracking(true);
    widget->installEventFilter(this);
    startTime.start();

    // Says the handler the start is done
    tao_event_handler->afterStart();
}


void EventCapture::stop()
// ----------------------------------------------------------------------------
//   Stop recording events
// ----------------------------------------------------------------------------
{
    // Says the handler capture is about to stop
    tao_event_handler->beforeStop();

    // disconnection
    foreach (QAction* act,  widget->parent()->findChildren<QAction*>())
    {
        disconnect(act, SIGNAL(triggered(bool)),
                   this, SLOT(recordAction(bool)));
    }
    widget->setMouseTracking(oldMouseTracking);
    widget->removeEventFilter(this);

    // Says the handler the stop is done
    tao_event_handler->afterStop();

    win->statusBar()->showMessage( "End event capture.");
}


void EventCapture::recordAction(bool )
// ----------------------------------------------------------------------------
//   Records triggered QActions.
// ----------------------------------------------------------------------------
{
    QAction* act = dynamic_cast<QAction*>(QObject::sender());
    if ( ! act) return;

    QString actName = act->objectName();
    if (actName.isEmpty()) return;

    int time = startTime.restart();

    TaoActionEvent *actionEvent = new TaoActionEvent(actName, time);
    tao_event_handler->add(actionEvent);
}


void EventCapture::recordColor(QColor color)
// ----------------------------------------------------------------------------
//   Record the color change event.
// ----------------------------------------------------------------------------
{
    TaoColorActionEvent * evt =
            new TaoColorActionEvent(QObject::sender()->objectName(),
                                     color.name(), color.alphaF(),
                                     startTime.restart());
    tao_event_handler->add(evt);
}


void EventCapture::recordFont(QFont font)
// ----------------------------------------------------------------------------
//   Record the font change event.
// ----------------------------------------------------------------------------
{
    TaoFontActionEvent * evt =
            new TaoFontActionEvent(QObject::sender()->objectName(),
                                    font.toString(), startTime.restart());
    tao_event_handler->add(evt);
}

void EventCapture::recordFile(QString file)
// ----------------------------------------------------------------------------
//   Record the font change event.
// ----------------------------------------------------------------------------
{
    if (file.isEmpty()) return;

    TaoFileActionEvent * evt =
            new TaoFileActionEvent(QObject::sender()->objectName(),
                                    file, startTime.restart());
    tao_event_handler->add(evt);
}


void EventCapture::finishedDialog(int result)
// ----------------------------------------------------------------------------
//   Record the result of the dialog box (and close event).
// ----------------------------------------------------------------------------
{
    QObject *sender = QObject::sender();
    if ( ! sender)
        return;

    QDialog * dialog = dynamic_cast<QDialog*>(sender);
    if (dialog)
    {
        disconnect(dialog, 0, this, 0);
    }
    TaoDialogActionEvent* evt =
            new TaoDialogActionEvent(sender->objectName(),
                                     result, startTime.restart());
    tao_event_handler->add(evt);

}


bool EventCapture::eventFilter(QObject */*obj*/, QEvent *evt)
// ----------------------------------------------------------------------------
//   Records all the events on widget
// ----------------------------------------------------------------------------
{
    switch(evt->type())
    {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        {
            QKeyEvent *e = (QKeyEvent*)evt;
            int delay = startTime.restart();
            TaoKeyEvent *keyEvent = new TaoKeyEvent (*e, delay);
            tao_event_handler->add(keyEvent);
            break;
        }
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
        {
            QMouseEvent *e = (QMouseEvent *)evt;
            int delay = startTime.restart();
            TaoMouseEvent *mouseEvent = new TaoMouseEvent(*e, delay);
            tao_event_handler->add(mouseEvent);
            break;
        }
//    case QEvent::Wheel:
//        {
//            QWheelEvent *e = (QWheelEvent *)evt;
//            int d = event->delta();
//            Qt::Orientation orientation = event->orientation();
//            // ICI mettre dx et dy de coter pour savoir ou est le centre.
//            //ATTENTION on n'a que les mouvements due a la roulette...
//            longlong dx = orientation == Qt::Horizontal ? d : 0;
//            longlong dy = orientation == Qt::Vertical   ? d : 0;
//            break;
//        }
    case QEvent::ChildPolished:
        {
            QChildEvent *e = (QChildEvent*)evt;
            QString childName = e->child()->objectName();
            if ( childName.contains("colorDialog"))
            {
                QColorDialog *diag = (QColorDialog*)e->child();
                connect(diag, SIGNAL(currentColorChanged(QColor)),
                        this, SLOT(recordColor(QColor)));
                connect(diag, SIGNAL(finished(int)),
                        this, SLOT(finishedDialog(int)));
            }
            else if (childName.contains("fontDialog"))
            {
                QFontDialog *diag = (QFontDialog*)e->child();
                connect(diag, SIGNAL(currentFontChanged(QFont)),
                        this, SLOT(recordFont(QFont)));
                connect(diag, SIGNAL(finished(int)),
                        this, SLOT(finishedDialog(int)));
            }
            else if (childName.contains("fileDialog") ||
                     childName.contains("QFileDialog"))
            {
                QFileDialog *diag = (QFileDialog*)e->child();
                connect(diag, SIGNAL(fileSelected(QString)),
                        this, SLOT(recordFile(QString)));
            }
        }
    default:
        break;
    }

    return false;

}


EventClient::EventClient(TaoEventHandler *handler, QGLWidget *glw):
// ----------------------------------------------------------------------------
//   Creates a player either on the provided glw or on the main QGLWidget
// ----------------------------------------------------------------------------
        synchroBasic(handler, glw)
{}


void EventClient::startClient()
// ----------------------------------------------------------------------------
//   Starts the player
// ----------------------------------------------------------------------------
{
    if ( ! win || ! widget ) return ;
    tao_event_handler->beforeStart();

    tao_event_handler->afterStart();
}


void EventClient::stop()
// ----------------------------------------------------------------------------
//  Stop the player
// ----------------------------------------------------------------------------
{
    tao_event_handler->beforeStop();
    tao->refreshOn(QEvent::Timer, -1.0);
    tao_event_handler->afterStop();
}
