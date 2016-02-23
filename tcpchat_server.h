#ifndef TCPCHAT_SERVER_H
#define TCPCHAT_SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QHostAddress>
#include <QMap>
#include <QDateTime>
#include <QList>
#include <QTimer>

class TcpChat_Server : public QObject
{
    Q_OBJECT
public:
    explicit TcpChat_Server(QObject *parent = 0);
    ~TcpChat_Server();

    void server_stop();
    void send_to_all(QString);

    QList<QString> getMessages()const{return this->Messages;}
    QList<QString> getClients()const;

signals:
    void s_update();

private slots:
    void newuser();
    void slotReadClient();
    void send_clients();

private:
    QTimer t_send_clients;
    QTcpServer *serv;
    int server_status;
    QMap<int, QTcpSocket*> SClients;
    QMap<int, quint16> Size;
    QList<QString> Messages;
};

#endif // TCPCHAT_SERVER_H
