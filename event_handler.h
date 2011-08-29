#ifndef TAO_EVENT_HANDLER_H
#define TAO_EVENT_HANDLER_H
// ****************************************************************************
//  event_handler.h						    Tao project
// ****************************************************************************
//
//   File Description:
//
//
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
//  (C) 2011 Catherine Burvelle <cathy@taodyne.com>
//  (C) 2011 Taodyne SAS
// ****************************************************************************

#include <QTcpServer>
#include <QDataStream>
#include <QList>
#include <QGLWidget>
#include "tao_control_event.h"
#include "main.h"

class TaoEventHandler : public QObject
{
public:
    virtual void beforeStart() = 0;
    virtual void afterStart() = 0;
    virtual void beforeStop() = 0;
    virtual void afterStop() = 0;

    virtual void add(TaoControlEvent *evt) = 0;
};


class TaoSynchro : public TaoEventHandler
{
    Q_OBJECT
public:
    TaoSynchro();
    virtual ~TaoSynchro();
    virtual void beforeStart();
    virtual void afterStart();
    virtual void beforeStop();
    virtual void afterStop();

    virtual void add(TaoControlEvent *evt);

protected slots:
    void registerClient();

protected:
    QList< QDataStream * > outList;

    QTcpServer * tcpServer;
};

class TaoSynchroClient : public TaoEventHandler
{
    Q_OBJECT
public:
    TaoSynchroClient(text serverName, int serverPort, QGLWidget *widget);
    virtual ~TaoSynchroClient();
    virtual void beforeStart();
    virtual void afterStart();
    virtual void beforeStop();
    virtual void afterStop();

    virtual void add(TaoControlEvent *evt);

protected slots:
    void readEvent();
    void displayError(QAbstractSocket::SocketError socketError);
protected:
    text serverName;
    int serverPort;

    QTcpSocket * tcpSocket;
    QGLWidget  * widget;
};



inline QString operator +(text s)
// ----------------------------------------------------------------------------
//   Quickly convert from text to QString
// ----------------------------------------------------------------------------
{
    return QString::fromUtf8(s.data(), s.length());
}

inline text operator +(QString s)
// ----------------------------------------------------------------------------
//   Quickly convert from QString to text
// ----------------------------------------------------------------------------
{
    return text(s.toUtf8().constData());
}


#endif // TAO_EVENT_HANDLER_H
