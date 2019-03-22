#ifndef TAOSYNCHRO_EVENTHANDLER_H
#define TAOSYNCHRO_EVENTHANDLER_H
// *****************************************************************************
// taosynchro_eventhandler.h                                       Tao3D project
// *****************************************************************************
//
// File description:
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
