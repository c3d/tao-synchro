// *****************************************************************************
// taosynchro_eventhandler.cpp                                     Tao3D project
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
#include "taosynchro_eventhandler.h"
#include "tao_control_event.h"
#include "event_capture.h"
#include <QMessageBox>
#include <QTcpSocket>

TaoSynchro::~TaoSynchro()
// ----------------------------------------------------------------------------
//  No more clean than afterStop.
// ----------------------------------------------------------------------------
{
    afterStop();
}


bool TaoSynchro::beforeStart()
// ----------------------------------------------------------------------------
//  Called before the startCapture operation
// ----------------------------------------------------------------------------
//  It starts and open a TCP server to listen on ANYv6 port 653000
//  Connect signal newConnection to slot registerClient to handle client request
{
    // start server
    tcpServer = new QTcpServer();
    if (!tcpServer->listen(QHostAddress::AnyIPv6, 65300)) {
        QMessageBox::critical(NULL, tr("Tao synchro Server"),
                              tr("Unable to start the server: %1.")
                              .arg(tcpServer->errorString()));
        IFTRACE(synchro)
                std::cerr << "Server on "
                << +tcpServer->serverAddress().toString()
                << " port: " << tcpServer->serverPort()
                << " NOT STARTED.\n";

        tcpServer->close();
        delete tcpServer;
        tcpServer = NULL;
        return false;
    }
    IFTRACE(synchro)
            std::cerr << "Starting server on "
            << +tcpServer->serverAddress().toString()
            << " port: " << tcpServer->serverPort() << std::endl;

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(registerClient()));
    return true;
}


bool TaoSynchro::afterStart()
// ----------------------------------------------------------------------------
//  Called after the startCapture operation
// ----------------------------------------------------------------------------
{
    return true;
}


bool TaoSynchro::beforeStop()
// ----------------------------------------------------------------------------
//  Called before the stopCapture operation
// ----------------------------------------------------------------------------
{
    return true;
}


bool TaoSynchro::afterStop()
// ----------------------------------------------------------------------------
//  Called after the stopCapture operation
// ----------------------------------------------------------------------------
{
    if (! tcpServer) return false;

    IFTRACE(synchro)
            std::cerr << "Stoping server on "
            << +tcpServer->serverAddress().toString()
            << " port: " << tcpServer->serverPort() << std::endl;
    // Stop listening
    tcpServer->close();

    // Close and clean each client connection
    QTcpSocket *soc = NULL;
    while( ! outList.empty())
    {
        soc = outList.back();
        outList.pop_back();
        soc->disconnectFromHost();
    }

    // clean memory
    outList.clear(); // should already be empty
    delete tcpServer;
    tcpServer = NULL;
    IFTRACE(synchro)
            std::cerr << "Server stopped\n";
    return true;
}


void TaoSynchro::add(TaoControlEvent *evt)
// ----------------------------------------------------------------------------
//  Deals with the new event
// ----------------------------------------------------------------------------
// TaoSynchro is now the evt owner and must manage its lifecycle.
{
    IFTRACE(synchro)
            std::cerr << +evt->toTaoCmd();
    // Serialize event into block QByteArray
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    // Reserve message size location
    out << (quint16)0;
    evt->serialize(out);
    // Set message size
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    // Send event to each client
    foreach (QTcpSocket *client, outList)
    {
        client->write(block);
        client->flush();
        IFTRACE(synchro)
                std::cerr << "\t Device flushed." << std::endl;
    }

    // End of evt life.
    delete evt;
}


void TaoSynchro::registerClient()
// ----------------------------------------------------------------------------
//  New client
// ----------------------------------------------------------------------------
{
    IFTRACE(synchro)
            std::cerr << "Server: New client request\n";
    if (!tcpServer) return;

    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    if ( ! clientConnection)
        return;

    if ( ! handShake(clientConnection) )
        return ;

    connect(clientConnection, SIGNAL(disconnected()),
            this, SLOT(removeClient()));

    outList.push_back(clientConnection);

    IFTRACE(synchro)
            std::cerr << "Server: Connection added\n";
}


void TaoSynchro::removeClient()
// ----------------------------------------------------------------------------
//  Unregistering the client that emit QTcpSocket::disconnected
// ----------------------------------------------------------------------------
{
    QTcpSocket * client = dynamic_cast<QTcpSocket *>(QObject::sender());
    if ( !client ) return;

    std::vector< QTcpSocket * >::iterator it;
    for ( it = outList.begin() ; it != outList.end(); it++)
    {
        if (*it == client)
        {
            outList.erase(it);
            IFTRACE(synchro)
                    std::cerr << "Server: Client " << +client->peerName()
                    << " removed\n";
            // Client MUST NOT be deleted here, but at end of event loop
            client->deleteLater();
            return;
        }
    }
}


bool TaoSynchro::handShake(QTcpSocket *client)
// ----------------------------------------------------------------------------
//  Synchrone handshake between client and server.
// ----------------------------------------------------------------------------
//  It is synchronized by the waitForReadyRead function of QTcpSocket.
{
    quint32 clientVersion = 0;
    quint32 clientMagic = 0;
    QDataStream out(client);
    out << (quint32) TAO_SYNCHRO_MAGIC ;
    out << (quint32) TAO_SYNCHRO_VERSION ;
    // wait up to 30s for data to be read, and then hope to got 4 bytes
    if ( ! client->waitForReadyRead() || client->bytesAvailable() < 8)
    {
        client->close();
        IFTRACE(synchro)
                std::cerr << "Server: Not enought byte to read.\n";
        return false;
    }
    out >> clientMagic;
    out >> clientVersion;
    //Check client version compatibility
    if (clientMagic != TAO_SYNCHRO_MAGIC ||
        clientVersion != TAO_SYNCHRO_VERSION)
    {
        client->close();
        IFTRACE(synchro)
                std::cerr << "Bad magic number " << clientMagic
                << " or server version " << TAO_SYNCHRO_VERSION
                << " not compatible with client version " << clientVersion
                << ".\n";
        return false;
    }
    return true;
}


TaoSynchroClient::TaoSynchroClient(text serverName, int serverPort ):
        serverName(serverName), serverPort(serverPort)
// ----------------------------------------------------------------------------
//  Client creation
// ----------------------------------------------------------------------------
{
    IFTRACE(synchro)
            std::cerr << "Client created\n";
    tcpSocket = new QTcpSocket();
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                 this, SLOT(displayError(QAbstractSocket::SocketError)));
}


TaoSynchroClient::~TaoSynchroClient()
// ----------------------------------------------------------------------------
//  Cleanup
// ----------------------------------------------------------------------------
{
    cleanup();
}


bool TaoSynchroClient::beforeStart()
// ----------------------------------------------------------------------------
//  Called before the startPlay operation
// ----------------------------------------------------------------------------
//  The handshake phase is synchrone using waitForReadyRead of QTcpSocket,
//  then signal readyRead is connected to readEvent.
{
    IFTRACE(synchro)
            std::cerr << "Starting client\n";

    if (! tcpSocket) return false;

    // Connection to host
    tcpSocket->connectToHost(+serverName, serverPort);
    QDataStream  in(tcpSocket);

    IFTRACE(synchro)
            std::cerr << "Client: Connecting to server" << serverName
            << " port " << serverPort << "\n";
    // Wait up to 30s for available data to read
    if ( !tcpSocket->waitForReadyRead() || tcpSocket->bytesAvailable() < 8)
    {
        cleanup();
        return false;
    }

    // Read and check Magic and version compatibility
    IFTRACE(synchro)
            std::cerr << "Client: reading magic and version.\n";
    quint32 magic = 0;
    quint32 svr_version = 0;
    in >> magic;
    in >> svr_version;
    if (magic != TAO_SYNCHRO_MAGIC || svr_version != TAO_SYNCHRO_VERSION)
    {
        in << (quint32) 0;
        cleanup();
        return false;
    }

    // Send back Magic and client version
    IFTRACE(synchro)
            std::cerr << "Client: sending magic and version.\n";
    in << (quint32) TAO_SYNCHRO_MAGIC;
    in << (quint32) TAO_SYNCHRO_VERSION;

    in.setVersion(QDataStream::Qt_4_7);

    // Connecting signal and slot
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readEvent()));
    IFTRACE(synchro)
            std::cerr << "Client: ready to listen to events.\n";
    return true;
}


bool TaoSynchroClient::afterStart()
// ----------------------------------------------------------------------------
//  Called after the startCapture operation
// ----------------------------------------------------------------------------
{
    return true;
}


bool TaoSynchroClient::beforeStop()
// ----------------------------------------------------------------------------
//  Called before the stopCapture operation
// ----------------------------------------------------------------------------
{
    return true;
}


bool TaoSynchroClient::afterStop()
// ----------------------------------------------------------------------------
//  Called after the stopCapture operation
// ----------------------------------------------------------------------------
{
    cleanup();
    return true;
}


void  TaoSynchroClient::cleanup()
// ----------------------------------------------------------------------------
//  Cleans the client : closes and deletes tcpSocket
// ----------------------------------------------------------------------------
{
    IFTRACE(synchro)
            std::cerr <<"TaoSynchroClient::cleanup()\n";
    if (! tcpSocket) return;
    tcpSocket->close();
    delete tcpSocket;
    tcpSocket = NULL;
}


void TaoSynchroClient::add(TaoControlEvent *evt)
// ----------------------------------------------------------------------------
//  Deals with the new event
// ----------------------------------------------------------------------------
{
    IFTRACE(synchro)
            std::cerr << +evt->toTaoCmd();

    evt->simulateNow(synchroBasic::base->widget);
    // end of evt life
    delete evt;
}


void TaoSynchroClient::readEvent()
// ----------------------------------------------------------------------------
//  Event ready to be read on the socket
// ----------------------------------------------------------------------------
{
    IFTRACE(synchro)
       std::cerr <<"->readEvent\n";

    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_7);

    while(tcpSocket->bytesAvailable() > 1)
    {
        quint16 size = 0;
        // Read 2 bytes and let them into the socket for next try
        // if not enought data to read.
        char buf[2];
        if (tcpSocket->peek(buf, 2 ) != 2)
        {
            IFTRACE(synchro)
               std::cerr <<"<- readEvent. Error peeking data size.\n";
            return;
        }
        // Convert buf into uint16
        QByteArray tmp(buf);
        QDataStream sz(tmp);
        sz >> size;
        if (tcpSocket->bytesAvailable() < size + 2)
        {
            IFTRACE(synchro)
               std::cerr <<"<- readEvent. Not enought data.\n";
            return;
        }

        // Read (and remove) the size before reading event.
        in >> size;

        // Read event
        IFTRACE(synchro)
                std::cerr <<"Event to be read.\n";
        TaoControlEvent *evt = TaoControlEvent::unserialize(in);
        add(evt);

    }
    IFTRACE(synchro)
       std::cerr <<"<- readEvent\n";
}


void TaoSynchroClient::displayError(QAbstractSocket::SocketError socketError)
// ----------------------------------------------------------------------------
//  Connection error handling
// ----------------------------------------------------------------------------
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(synchroBasic::base->widget,
                                 tr("Synchro Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(synchroBasic::base->widget,
                                 tr("Synchro Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the Synchro server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(synchroBasic::base->widget,
                                 tr("Synchro Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }
}
