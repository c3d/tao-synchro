// ****************************************************************************
//  event_handler.cpp						    Tao project
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
#include "event_handler.h"
#include "tao_control_event.h"
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
    QDataStream *out = NULL;
    while((out = outList.back()))
    {
        outList.pop_back();
        QTcpSocket * soc = (QTcpSocket * )out->device();
        soc->disconnectFromHost();
        delete out;
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
            std::cerr << +evt->toTaoCmd() << std::endl;
    foreach (QDataStream *out, outList)
    {
        evt->serialize(*out);
        ((QTcpSocket*)out->device())->flush();
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

    QDataStream *out = handShake(clientConnection);
    if ( ! out )
        return ;

    connect(clientConnection, SIGNAL(disconnected()),
            this, SLOT(removeClient()));

    outList.push_back(out);

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

    std::vector< QDataStream * >::iterator it;
    for ( it = outList.begin() ; it != outList.end(); it++)
    {
        QDataStream *tmp = *it;
        if (tmp->device() == client)
        {
            outList.erase(it);
            delete tmp;
            IFTRACE(synchro)
                    std::cerr << "Server: Client " << +client->peerName()
                    << " removed\n";
            // Client MUST NOT be deleted here, but at end of event loop
            client->deleteLater();
            return;
        }
    }
}


QDataStream *TaoSynchro::handShake(QTcpSocket *client)
// ----------------------------------------------------------------------------
//  Synchrone handshake between client and server.
// ----------------------------------------------------------------------------
//  It is synchronized by the waitForReadyRead function of QTcpSocket.
{
    quint32 clientVersion = 0;
    quint32 clientMagic = 0;
    QDataStream *out = new QDataStream(client);
    (*out) << (quint32) TAO_SYNCHRO_MAGIC ;
    (*out) << (quint32) TAO_SYNCHRO_VERSION ;
    // wait up to 30s for data to be read, and then hope to got 4 bytes
    if ( ! client->waitForReadyRead() || client->bytesAvailable() < 8)
    {
        client->close();
        delete out;
        IFTRACE(synchro)
                std::cerr << "Server: Not enought byte to read.\n";
        return NULL;
    }
    (*out) >> clientMagic;
    (*out) >> clientVersion;
    //Check client version compatibility
    if (clientMagic != TAO_SYNCHRO_MAGIC ||
        clientVersion != TAO_SYNCHRO_VERSION)
    {
        client->close();
        delete out;
        IFTRACE(synchro)
                std::cerr << "Bad magic number " << clientMagic
                << " or server version " << TAO_SYNCHRO_VERSION
                << " not compatible with client version " << clientVersion
                << ".\n";
        return NULL;
    }
    out->setVersion(QDataStream::Qt_4_7);
    return out;
}


TaoSynchroClient::TaoSynchroClient(text serverName, int serverPort,
                                   QGLWidget *widget):
        serverName(serverName), serverPort(serverPort), widget(widget)
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
    in.setDevice(tcpSocket);

    IFTRACE(synchro)
            std::cerr << "Client: Connected to server" << serverName
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
            std::cerr << +evt->toTaoCmd() << std::endl;

    evt->simulateNow(widget);
    // end of evt life
    delete evt;
}


void TaoSynchroClient::readEvent()
// ----------------------------------------------------------------------------
//  Event ready to be read on the socket
// ----------------------------------------------------------------------------
{
    IFTRACE(synchro)
            std::cerr <<"Event to be read\n";

    TaoControlEvent *evt = TaoControlEvent::unserialize(in);
    add(evt);
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
        QMessageBox::information(widget, tr("Synchro Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(widget, tr("Synchro Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the Synchro server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(widget, tr("Synchro Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }
}
