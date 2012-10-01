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
