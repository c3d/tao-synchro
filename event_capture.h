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
#include "tao/module_api.h"

#include <QGLWidget>
#include <QMainWindow>
#include <QTime>
#include <QSize>

struct synchroBasic : QObject
{
    Q_OBJECT

    public:
    synchroBasic(TaoEventHandler * handler,
                 QGLWidget *widget = NULL);
    virtual ~synchroBasic(){delete tao_event_handler;}
    void unproject(int x, int y, int z, int *ux, int *uy, int *uz);
    void project (int x, int y, int z, int *px, int *py, int *pz);

    virtual void stop()=0;

    TaoEventHandler * tao_event_handler;
    QGLWidget       * widget;
    QMainWindow     * win ;
    QSize             winSize;

    static synchroBasic   * base;
    const static Tao::ModuleApi * tao;
};

struct EventCapture : public synchroBasic
// ----------------------------------------------------------------------------
//   Capturing events
// ----------------------------------------------------------------------------
{
Q_OBJECT

public:
    EventCapture(TaoEventHandler *handler, QGLWidget *widget = NULL);

    void startCapture();
    virtual void stop();

    // Spying events on widget
    bool eventFilter(QObject *obj, QEvent *evt);

public slots:
    void recordAction(bool triggered);
    void recordColor(QColor color);
    void recordFont(QFont font);
    void recordFile(QString filename);
    void finishedDialog(int result);

public:
    QTime      startTime;

private:
    bool oldMouseTracking;

};


struct EventClient : public synchroBasic
// ----------------------------------------------------------------------------
//   Playing events
// ----------------------------------------------------------------------------
{
Q_OBJECT

public:
    EventClient(TaoEventHandler *handler, QGLWidget *widget = NULL);

    void startClient();
    virtual void stop();

};
#endif // EVENTCAPTURE_H
