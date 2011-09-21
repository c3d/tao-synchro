// ****************************************************************************
//  event_handler.cpp						    Tao project
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
#include "event_handler.h"

#include "tao_control_event.h"
#include <QMessageBox>
#include <QTcpSocket>

TaoSynchro::TaoSynchro(): tcpServer(NULL)
{
}

TaoSynchro::~TaoSynchro()
{
    afterStop();
}
void TaoSynchro::beforeStart()
// ----------------------------------------------------------------------------
//  Called before the startCapture operation
// ----------------------------------------------------------------------------
{
    // start server
    tcpServer = new QTcpServer();
    if (!tcpServer->listen(QHostAddress::LocalHost, 65300)) {
        QMessageBox::critical(NULL, tr("Tao synchro Server"),
                              tr("Unable to start the server: %1.")
                              .arg(tcpServer->errorString()));
        tcpServer->close();
        delete tcpServer;
        tcpServer = NULL;
        return;
    }
    std::cerr << "Server " << +tcpServer->serverAddress().toString()
            << ": " << tcpServer->serverPort() << std::endl;

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(registerClient()));

}


void TaoSynchro::afterStart()
// ----------------------------------------------------------------------------
//  Called after the startCapture operation
// ----------------------------------------------------------------------------
{
}


void TaoSynchro::beforeStop()
// ----------------------------------------------------------------------------
//  Called before the stopCapture operation
// ----------------------------------------------------------------------------
{
}


void TaoSynchro::afterStop()
// ----------------------------------------------------------------------------
//  Called after the stopCapture operation
// ----------------------------------------------------------------------------
{
    if (! tcpServer) return;

    // Stop listening
    tcpServer->close();

    // Close and clean each client connection
    foreach (QDataStream *out, outList)
    {
        QTcpSocket * soc = (QTcpSocket * )out->device();
        soc->disconnectFromHost();
        delete out;
    }

    // clean memory
    outList.clear();
    delete tcpServer;
    tcpServer = NULL;

}


void TaoSynchro::add(TaoControlEvent *evt)
// ----------------------------------------------------------------------------
//  Deals with the new event
// ----------------------------------------------------------------------------
{
    std::cerr << +evt->toTaoCmd() << std::endl;

    foreach (QDataStream *out, outList)
        evt->serialize(*out);
}


void TaoSynchro::registerClient()
// ----------------------------------------------------------------------------
//  New client
// ----------------------------------------------------------------------------
{
    std::cerr << "New Connection !\n";
    if (!tcpServer) return;

    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    if ( ! clientConnection)
        return;

    std::cerr << "New Connection 2!\n";

    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));

    QDataStream *out = new QDataStream(clientConnection);
    out->setVersion(QDataStream::Qt_4_0);
    outList.push_back(out);
}





TaoSynchroClient::TaoSynchroClient(text serverName, int serverPort,
                                   QGLWidget *widget):
        serverName(serverName), serverPort(serverPort), widget(widget)
{
    tcpSocket = new QTcpSocket();
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readEvent()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                 this, SLOT(displayError(QAbstractSocket::SocketError)));
}

TaoSynchroClient::~TaoSynchroClient()
{
    afterStop();
}
void TaoSynchroClient::beforeStart()
// ----------------------------------------------------------------------------
//  Called before the startCapture operation
// ----------------------------------------------------------------------------
{
    if (! tcpSocket) return;

    tcpSocket->connectToHost(QHostAddress::LocalHost, 65300);

    //    tcpSocket->connectToHost(+serverName, serverPort);
}


void TaoSynchroClient::afterStart()
// ----------------------------------------------------------------------------
//  Called after the startCapture operation
// ----------------------------------------------------------------------------
{
}


void TaoSynchroClient::beforeStop()
// ----------------------------------------------------------------------------
//  Called before the stopCapture operation
// ----------------------------------------------------------------------------
{

}


void TaoSynchroClient::afterStop()
// ----------------------------------------------------------------------------
//  Called after the stopCapture operation
// ----------------------------------------------------------------------------
{
    if (! tcpSocket) return;
    disconnect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readEvent()));
    tcpSocket->close();
    delete tcpSocket;
    tcpSocket = NULL;
}



void TaoSynchroClient::add(TaoControlEvent *evt)
// ----------------------------------------------------------------------------
//  Deals with the new event
// ----------------------------------------------------------------------------
{
    std::cerr << +evt->toTaoCmd() << std::endl;

    evt->simulateNow(widget);

    // TODO
    // delete evt
    // not yet should register our self on willbedeleted of the evt->event for mouse and kb;
}


void TaoSynchroClient::readEvent()
// ----------------------------------------------------------------------------
//  Event ready to be read on the socket
// ----------------------------------------------------------------------------
{
    std::cerr <<"Event to be read\n";
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    TaoControlEvent *evt = TaoControlEvent::unserialize(in);
    add(evt);
}

void TaoSynchroClient::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(widget, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(widget, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(widget, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }

}
