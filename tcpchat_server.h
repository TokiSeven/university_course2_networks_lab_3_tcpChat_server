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

struct CLIENT
{
    QString name = "NoName";
    bool auth = false;
    QTcpSocket *soc;
    quint16 size = 0;
};

class TcpChat_Server : public QObject
{
    Q_OBJECT
public:
    explicit TcpChat_Server(QObject *parent = 0);
    ~TcpChat_Server();

    void server_stop();
    void send_to_all(QString, QString);

    template <typename T>
    void send_to_one(QString cmd, QTcpSocket *socket, T data);

    QList<QString> getMessages()const{return this->Messages;}
    QList<QString> getClients()const;

signals:
    void s_update();

private slots:
    void newuser();
    void slotReadClient();
    void send_clients();
    void deleteuser();
    void sl_update();

private:
    QTimer t_send_clients;
    QTcpServer *serv;
    int server_status;
    QMap<int, CLIENT> SClients;
    QList<QString> Messages;
};

template <typename T>
void TcpChat_Server::send_to_one(QString cmd, QTcpSocket *socket, T data)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    //резервируем 2 байта для размера блока.
    out << (quint16)0;
    out << cmd;
    out << data;

    //возваращаемся в начало
    out.device()->seek(0);

    //вписываем размер блока на зарезервированное место
    out << (quint16)(block.size() - sizeof(quint16));

    socket->write(block);
}

#endif // TCPCHAT_SERVER_H
