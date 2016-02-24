#include "tcpchat_server.h"

TcpChat_Server::TcpChat_Server(QObject *parent) : QObject(parent)
{
    this->serv = new QTcpServer;
    connect(this->serv, SIGNAL(newConnection()), this, SLOT(newuser()));

    t_send_clients.setInterval(1000);

    if (!this->serv->listen(QHostAddress::Any, 4343))
    {
        server_status = 0;
        qDebug() <<  QObject::tr("Unable to start the server: %1.").arg(this->serv->errorString());
    }
    else
    {
        server_status = 1;
        qDebug() << QString::fromUtf8("Server was started!");
        t_send_clients.start();
        connect(&t_send_clients, SIGNAL(timeout()), this, SLOT(send_clients()));
    }
}

TcpChat_Server::~TcpChat_Server()
{
    delete this->serv;
}

void TcpChat_Server::newuser()
{
    if (server_status)
    {
        qDebug() << QString::fromStdString("New connection!");

        QTcpSocket* clientSocket = serv->nextPendingConnection();

        int idusersocs = clientSocket->socketDescriptor();

        SClients[idusersocs] = clientSocket;
        Size[idusersocs] = 0;

        connect(SClients[idusersocs], SIGNAL(readyRead()), this, SLOT(slotReadClient()));
        connect(SClients[idusersocs], SIGNAL(disconnected()), this, SLOT(deleteuser()));

        emit s_update();
    }
}
void TcpChat_Server::deleteuser()
{
    // �������� ������ ������, ������� ������ ������ ����
    QTcpSocket* clientSocket = (QTcpSocket*)sender();

    // �������� ����������, ��� ����, ���� � ������ �������� ������ ������� ��� �� �����
    int idusersocs = clientSocket->socketDescriptor();

    // ������� �����
    clientSocket->close();
    // ������ ������ ������ �� �����
    SClients.remove(idusersocs);

    emit s_update();
}

void TcpChat_Server::server_stop()
{
    if (server_status)
    {
        foreach(int i, SClients.keys())
        {
            QTextStream os(SClients[i]);
            os.setAutoDetectUnicode(true);
            os << QDateTime::currentDateTime().toString() << "\n";
            SClients[i]->close();
            SClients.remove(i);
        }
        serv->close();
        qDebug() << QString::fromStdString("Server was stopped!");
        server_status = 0;
    }
}

void TcpChat_Server::slotReadClient()
{
    // �������� ������ ������, ������� ������ ������ ����
    QTcpSocket* clientSocket = (QTcpSocket*)sender();

    // �������� ����������, ��� ����, ���� � ������ �������� ������ ������� ��� �� �����
    int idusersocs = clientSocket->socketDescriptor();

    //��� �������������� ������ �� �������
    QDataStream in(clientSocket);

    //���� ��������� ����� ���� ������ 2 ����� ��� ��� ������
    if (Size[idusersocs] == 0)
    {
        //���� ������ ������ 2 ���� ���� ���� ����� 2 �����
        if (clientSocket->bytesAvailable() < (int)sizeof(quint16))
            return;

        //��������� ������ (2 �����)
        quint16 ss;
        in >> ss;
        Size[idusersocs] = ss;
    }

    //���� ���� ���� ������� ���������
    if (clientSocket->bytesAvailable() < Size[idusersocs])
        return;
    else
        Size[idusersocs] = 0;//����� ��������� ����� ����

    QString message;
    in >> message;
    message = clientSocket->peerAddress().toString() + QString::fromStdString(": ") + message;
    Messages.append(message);

    this->send_to_all(message);

    emit s_update();
}

void TcpChat_Server::send_to_all(QString mess)
{
    if (this->server_status)
    {
        qDebug() << "Sending message";
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        //����������� 2 ����� ��� ������� �����.
        out << (quint16)0;
        out << QString::fromStdString("MESSAGE");
        out << mess;

        //������������� � ������
        out.device()->seek(0);

        //��������� ������ ����� �� ����������������� �����
        out << (quint16)(block.size() - sizeof(quint16));

        foreach(int i, SClients.keys())
            SClients[i]->write(block);
    }
}

void TcpChat_Server::send_clients()
{
    if (this->server_status)
    {
        qDebug() << "Sending clients";
        //from qmap to qlist
        QList<QHostAddress> addr;
        foreach(int i, SClients.keys())
            addr.append(SClients[i]->peerAddress());

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        //����������� 2 ����� ��� ������� �����.
        out << (quint16)0;
        out << QString::fromStdString("CLIENTS");
        out << addr;

        //������������� � ������
        out.device()->seek(0);

        //��������� ������ ����� �� ����������������� �����
        out << (quint16)(block.size() - sizeof(quint16));

        foreach(int i, SClients.keys())
            SClients[i]->write(block);
    }
}

QList<QString> TcpChat_Server::getClients()const
{
    QList<QString> res;
    foreach(int i, SClients.keys())
        res.append(SClients[i]->peerAddress().toString());
    return res;
}
