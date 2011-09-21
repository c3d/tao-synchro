// ****************************************************************************
//  EventCapture.cpp						    Tao project
// ****************************************************************************
//
//   File Description:
//
//     The test tools
//
//
//
//
//
//
//
//
// ****************************************************************************
// This software is property of Taodyne SAS - Confidential
// Ce logiciel est la propriété de Taodyne SAS - Confidentiel
//  (C) 2010 Catherine Burvelle <cathy@taodyne.com>
//  (C) 2010 Taodyne SAS
// ****************************************************************************

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

EventCapture::EventCapture(TaoEventHandler *handler, QGLWidget *glw) :
// ----------------------------------------------------------------------------
//   Creates a new test.
// ----------------------------------------------------------------------------
    tao_event_handler(handler), widget(glw),  win(NULL)
{

    if ( ! glw )
        foreach (QWidget *w, QApplication::topLevelWidgets())
            if ((win = dynamic_cast<QMainWindow*>(w)) != NULL)
                if ((widget = dynamic_cast<QGLWidget *>(win->centralWidget()) ) != NULL)
                    break;
}


void EventCapture::startCapture()
// ----------------------------------------------------------------------------
//   Start recording a sequence of events
// ----------------------------------------------------------------------------
{
    win->statusBar()->showMessage("Start event capture.");
    tao_event_handler->beforeStart();
// SYNCH
    // start du serveur ?
    winSize = win->size();

    //connection
    foreach (QAction* act,  widget->parent()->findChildren<QAction*>())
    {
        if (act->objectName().startsWith("toolbar:test"))
            continue;
         connect(act, SIGNAL(triggered(bool)), this, SLOT(recordAction(bool)));
    }
    widget->installEventFilter(this);
    startTime.start();

    tao_event_handler->afterStart();

// SYNCH
    // Envoyer un ordre de synchro (No page, page time, doc time, win size)

}


void EventCapture::stopCapture()
// ----------------------------------------------------------------------------
//   Stop recording events
// ----------------------------------------------------------------------------
{
// SYNCH
    // Ordre de fin // shutdown du serveur ?
    tao_event_handler->beforeStop();

    // disconnection
    foreach (QAction* act,  widget->parent()->findChildren<QAction*>())
    {
        disconnect(act, SIGNAL(triggered(bool)),
                   this, SLOT(recordAction(bool)));
    }
    widget->removeEventFilter(this);

    tao_event_handler->afterStop();

    win->statusBar()->showMessage( "End event capture.");
}


void EventCapture::recordAction(bool )
// ----------------------------------------------------------------------------
//   Records actions.
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
                                     color.name(), startTime.restart());
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
    if (sender)
    {
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
}

/*
void EventCapture::checkNow()
// ----------------------------------------------------------------------------
//   Records a check point and the view.
// ----------------------------------------------------------------------------
{
    QImage shot = widget->grabFrameBuffer(true);
    TaoCheckEvent *check = new TaoCheckEvent(checkPointList.size(),
                                             shot,
                                             startTime.restart());
    tao_event_handler->add(check);
}

*/

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
    case QEvent::ChildPolished:
        {
            QChildEvent *e = (QChildEvent*)evt;
            QString childName = e->child()->objectName();
            std::cerr<< "Object polished " << +childName << std::endl; // CaB
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
//                connect(diag, SIGNAL(finished(int)),
//                        this, SLOT(finishedDialog(int)));
            }
        }
    default:
        break;
    }

    return false;

}


/*
void EventCapture::addKeyPress(Qt::Key qtKey,
                              Qt::KeyboardModifiers modifiers,
                              int msecs)
// ----------------------------------------------------------------------------
// Add a key press event to the list of action
// ----------------------------------------------------------------------------
{
    testList.addKeyPress(qtKey, modifiers, msecs);
}


void EventCapture::addKeyRelease(Qt::Key qtKey,
                                Qt::KeyboardModifiers modifiers,
                                int msecs)
// ----------------------------------------------------------------------------
// Add a key release event to the list of action
// ----------------------------------------------------------------------------
{
    testList.addKeyRelease(qtKey, modifiers, msecs);
}


void EventCapture::addMousePress(Qt::MouseButton button,
                                Qt::KeyboardModifiers modifiers,
                                QPoint pos, int delay )
// ----------------------------------------------------------------------------
// Add a mouse press event to the list of action
// ----------------------------------------------------------------------------
{
    testList.addMousePress(button, modifiers, pos, delay);
}


void EventCapture::addMouseRelease(Qt::MouseButton button,
                                  Qt::KeyboardModifiers modifiers,
                                  QPoint pos, int delay )
// ----------------------------------------------------------------------------
// Add a mouse release event to the list of action
// ----------------------------------------------------------------------------
{
    testList.addMouseRelease(button, modifiers, pos, delay);

}


void EventCapture::addMouseMove(Qt::MouseButtons buttons,
                               Qt::KeyboardModifiers modifiers,
                               QPoint pos, int delay)
// ----------------------------------------------------------------------------
// Add a mouse move event to the list of action
// ----------------------------------------------------------------------------
{
    testList.append(new TestMouseMoveEvent(buttons, modifiers, pos, delay));
}


void EventCapture::addMouseDClick(Qt::MouseButton button,
                                 Qt::KeyboardModifiers modifiers,
                                 QPoint pos, int delay)
// ----------------------------------------------------------------------------
// Add a mouse double click event to the list of action
// ----------------------------------------------------------------------------
{
    testList.addMouseDClick(button, modifiers, pos, delay);
}


TestActionEvent * EventCapture::addAction(QString actName, int delay)
// ----------------------------------------------------------------------------
// Add an action to be replayed.
// ----------------------------------------------------------------------------
{
    TestActionEvent* evt = new TestActionEvent(actName, delay);
    testList.append(evt);
    return evt;
}


TestCheckEvent * EventCapture::addCheck( int num, int delay)
// ----------------------------------------------------------------------------
// Add a mouse move event to the list of action
// ----------------------------------------------------------------------------
{
    TestCheckEvent * evt = new TestCheckEvent(num, delay);
    testList.append(evt);
    return evt;
}


TestColorActionEvent * EventCapture::addColor(QString diagName,
                                             QString colName,
                                             int delay)
// ----------------------------------------------------------------------------
// Add an action to be replayed.
// ----------------------------------------------------------------------------
{
    TestColorActionEvent * evt = new TestColorActionEvent(diagName,
                                                          colName,
                                                          delay);
    testList.append(evt);
    return evt;
}


TestFontActionEvent * EventCapture::addFont(QString diagName,
                                           QString ftName, int delay)
// ----------------------------------------------------------------------------
// Add an action to be replayed.
// ----------------------------------------------------------------------------
{
    TestFontActionEvent * evt = new TestFontActionEvent(diagName,
                                                        ftName, delay);
    testList.append(evt);
    return evt;
}


TestFileActionEvent* EventCapture::addFile(QString diagName,
                                          QString fileName, int delay)
// ----------------------------------------------------------------------------
// Add an action to be replayed.
// ----------------------------------------------------------------------------
{
    TestFileActionEvent * evt = new TestFileActionEvent(diagName,
                                                        fileName, delay);
    testList.append(evt);
    return evt;
}


TestDialogActionEvent* EventCapture::addDialogClose(QString objName,
                                                   int result,  int delay)
// ----------------------------------------------------------------------------
// Add an action to be replayed.
// ----------------------------------------------------------------------------
{
    TestDialogActionEvent * evt = new TestDialogActionEvent(objName,
                                                            result, delay);
    testList.append(evt);
    return evt;
}
*/

EventClient::EventClient(TaoEventHandler *handler, QGLWidget *glw):
// ----------------------------------------------------------------------------
//   Creates a new test.
// ----------------------------------------------------------------------------
        tao_event_client(handler), widget(glw),  win(NULL)
{
    if ( ! glw )
        foreach (QWidget *w, QApplication::topLevelWidgets())
            if ((win = dynamic_cast<QMainWindow*>(w)) != NULL)
                if ((widget = dynamic_cast<QGLWidget *>(win->centralWidget()) ) != NULL)
                    break;
}

void EventClient::startClient()
{
    tao_event_client->beforeStart();

    tao_event_client->afterStart();
}
void EventClient::stopClient()
{
    tao_event_client->beforeStop();

    tao_event_client->afterStop();
}
