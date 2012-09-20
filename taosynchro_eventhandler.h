#ifndef TAOSYNCHRO_EVENTHANDLER_H
#define TAOSYNCHRO_EVENTHANDLER_H
// ****************************************************************************
//  event_handler.h						    Tao project
// ****************************************************************************
//
//   File Description:
//     TaoEventHandler is the base class that deals with event carriing or
//   storing, etc... Subclass it to get a master side handler and a client
//   side one. It is used by EventCapture.
//
//     The first pair of subclasses deals with event transfer over network.
//   TaoSynchro is the master side class and TaoSynchroClient is the client
//   side one. The master capture and send events over the network and clients
//   receive and immedialtely play the event.
//
//     Another pair of subclass (TO BE DONE) will handle file storage for
//   playback to replace TaoTester.
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
#include "event_handler.h"
#include "main.h"

#define TAO_SYNCHRO_MAGIC       0xB0B0DAD1
#define TAO_SYNCHRO_UKN_VERSION 0x00000000
#define TAO_SYNCHRO_VERSION_1   0x00000001  // Using QDataStream::Qt_4_7
#define TAO_SYNCHRO_VERSION     TAO_SYNCHRO_VERSION_1


class TaoSynchro : public TaoEventHandler
// ----------------------------------------------------------------------------
//  Server class for synchro over network
// ----------------------------------------------------------------------------
//     TaoSynchro is a tcp server listening for client connections. The
//  signal-slot interface of Qt is used. No need of a specific thread.
//  The handshake is very simple : exchange of magic number and version.
{
    Q_OBJECT
public:
    TaoSynchro(): tcpServer(NULL) {}
    virtual ~TaoSynchro();
    virtual bool beforeStart();
    virtual bool afterStart();
    virtual bool beforeStop();
    virtual bool afterStop();

    virtual void add(TaoControlEvent *evt);

protected slots:
    void registerClient();
    void removeClient();

protected:
    bool handShake(QTcpSocket *client);


protected:
    std::vector< QTcpSocket * > outList;

    QTcpServer * tcpServer;
};


class TaoSynchroClient : public TaoEventHandler
// ----------------------------------------------------------------------------
//  Client class for synchro over network
// ----------------------------------------------------------------------------
//     TaoSynchroClient use a tcp connection to connect the server. The
//  signal-slot interface of Qt is used. No need of a specific thread.
//  The handshake is very simple : exchange of magic number and version.
//  After the handshake the server send serialized events to clients.
{
    Q_OBJECT
public:
    TaoSynchroClient(text serverName, int serverPort);
    virtual ~TaoSynchroClient();
    virtual bool beforeStart();
    virtual bool afterStart();
    virtual bool beforeStop();
    virtual bool afterStop();

    virtual void add(TaoControlEvent *evt);

protected slots:
    void readEvent();
    void displayError(QAbstractSocket::SocketError socketError);

protected:
    void  cleanup();

protected:
    text serverName;
    int serverPort;

    QTcpSocket * tcpSocket;
};


#endif // TAOSYNCHRO_EVENTHANDLER_H
