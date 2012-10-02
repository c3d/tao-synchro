// ****************************************************************************
//  tao_synchro.cpp						    Tao project
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
#ifndef STARTCLIENT_H
#define STARTCLIENT_H

#include <QDialog>
#include <QString>

namespace Ui {
class StartClient;
}

class StartClient : public QDialog
{
    Q_OBJECT

public:
    explicit StartClient(QString name, int port, QWidget *parent = 0);
    ~StartClient();
    QString hostname();
    int port();

private:
    Ui::StartClient *ui;
};

#endif // STARTCLIENT_H
