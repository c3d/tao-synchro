#include "startclient.h"
#include "ui_startclient.h"

StartClient::StartClient(QString name, int port, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartClient)
{
    ui->setupUi(this);
    ui->lineEdit->setText(name);
    ui->portEdit->setText(QString::number(port));
}


StartClient::~StartClient()
{
    delete ui;
}


QString StartClient::hostname()
{
    return ui->lineEdit->text();
}


int StartClient::port()
{
    return ui->portEdit->text().toInt();
}
