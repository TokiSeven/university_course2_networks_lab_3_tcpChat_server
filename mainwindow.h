#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tcpchat_server.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void sl_update();
    void on_button_stop_clicked();

private:
    Ui::MainWindow *ui;
    TcpChat_Server *serv;
};

#endif // MAINWINDOW_H
