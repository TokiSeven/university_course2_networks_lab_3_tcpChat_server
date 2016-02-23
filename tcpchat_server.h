#ifndef TCPCHAT_SERVER_H
#define TCPCHAT_SERVER_H

#include <QObject>
#include <QTcpServer>

class TcpChat_Server : public QObject
{
    Q_OBJECT
public:
    explicit TcpChat_Server(QObject *parent = 0);

signals:

public slots:
};

#endif // TCPCHAT_SERVER_H
