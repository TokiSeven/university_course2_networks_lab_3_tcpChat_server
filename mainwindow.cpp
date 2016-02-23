#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serv = new TcpChat_Server;
    connect(serv, SIGNAL(s_update()), this, SLOT(sl_update()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_stop_clicked()
{
    serv->server_stop();
    this->close();
}

void MainWindow::sl_update()
{
    this->ui->list_clients->clear();
    this->ui->list_messages->clear();

    QList<QString> mess = this->serv->getMessages();
    QList<QString> cl = this->serv->getClients();

    for (int i = 0; i < mess.size(); i++)
        ui->list_messages->addItem(mess[i]);
    for (int i = 0; i < cl.size(); i++)
        ui->list_clients->addItem(cl[i]);
}
