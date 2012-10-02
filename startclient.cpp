// ****************************************************************************
//  startclient.cpp						    Tao project
// ****************************************************************************
//
//   File Description:
//
//     XL interface for synchronization feature.
//     GUI to request master IP address and port.
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
#include "startclient.h"
#include "ui_startclient.h"

StartClient::StartClient(QString name, int port, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartClient)
// ----------------------------------------------------------------------------
//  Build the GUI based on the given name and port
// ----------------------------------------------------------------------------
{
    ui->setupUi(this);
    ui->lineEdit->setText(name);
    ui->portEdit->setText(QString::number(port));
}


StartClient::~StartClient()
// ----------------------------------------------------------------------------
//  Destructor
// ----------------------------------------------------------------------------
{
    delete ui;
}


QString StartClient::hostname()
// ----------------------------------------------------------------------------
//  Return the content of the hostname field
// ----------------------------------------------------------------------------
{
    return ui->lineEdit->text();
}


int StartClient::port()
// ----------------------------------------------------------------------------
//  Return the content of the port field if valid and 65300 otherwise.
// ----------------------------------------------------------------------------
{
    int p = ui->portEdit->text().toInt();
    if (p <= 0 )
        return 65300;
    return p;
}
