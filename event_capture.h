#ifndef EVENTCAPTURE_H
#define EVENTCAPTURE_H
// ****************************************************************************
//  event_capture.h					        Tao project
// ****************************************************************************
//
//   File Description:
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
// ****************************************************************************
// This software is property of Taodyne SAS - Confidential
// Ce logiciel est la propriété de Taodyne SAS - Confidentiel
//  (C) 2010 Catherine Burvelle <cathy@taodyne.com>
//  (C) 2010 Taodyne SAS
// ****************************************************************************

#include "main.h"
#include "event_handler.h"

#include <QGLWidget>
#include <QMainWindow>
#include <QTime>
#include <QSize>


struct EventCapture : public QObject
// ----------------------------------------------------------------------------
//   Capturing events
// ----------------------------------------------------------------------------
{
Q_OBJECT

public:
    EventCapture(TaoEventHandler *handler, QGLWidget *widget = NULL);

    void startCapture();
    void stopCapture();

    // Spying events on widget
    bool eventFilter(QObject *obj, QEvent *evt);

public slots:
    void recordAction(bool triggered);
    void recordColor(QColor color);
    void recordFont(QFont font);
    void recordFile(QString filename);
    void finishedDialog(int result);


public:
    TaoEventHandler * tao_event_handler;

    QGLWidget *widget;
    QTime      startTime;

    QMainWindow * win ;
    QSize         winSize;


};


struct EventClient : public QObject
// ----------------------------------------------------------------------------
//   Playing events
// ----------------------------------------------------------------------------
{
Q_OBJECT

public:
    EventClient(TaoEventHandler *handler, QGLWidget *widget = NULL);

    void startClient();
    void stopClient();

    TaoEventHandler * tao_event_client;

    QGLWidget *widget;

    QMainWindow * win ;
    QSize         winSize;

};
#endif // EVENTCAPTURE_H
