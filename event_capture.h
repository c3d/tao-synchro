#ifndef EVENTCAPTURE_H
#define EVENTCAPTURE_H
// *****************************************************************************
// event_capture.h                                                 Tao3D project
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
