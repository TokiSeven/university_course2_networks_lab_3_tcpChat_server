#include "tcpchat_server.h"

TcpChat_Server::TcpChat_Server(QObject *parent) : QObject(parent)
{
    this->serv = new QTcpServer;
    connect(this->serv, SIGNAL(newConnection()), this, SLOT(newuser()));
    connect(this, SIGNAL(s_update()), this, SLOT(sl_update()));

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
        QTcpSocket* clientSocket = serv->nextPendingConnection();

        int idusersocs = clientSocket->socketDescriptor();

        SClients[idusersocs].soc = clientSocket;

        connect(SClients[idusersocs].soc, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
        connect(SClients[idusersocs].soc, SIGNAL(disconnected()), this, SLOT(deleteuser()));

        emit s_update();
    }
}
void TcpChat_Server::deleteuser()
{
    // ѕолучаем объект сокета, который вызвал данный слот
    QTcpSocket* clientSocket = (QTcpSocket*)sender();

    // ѕолучаем дескриптор, дл€ того, чтоб в случае закрыти€ сокета удалить его из карты
    int idusersocs = clientSocket->socketDescriptor();

    // ”далим объект сокета из карты
    clientSocket->close();

    emit s_update();
}

void TcpChat_Server::sl_update()
{
    foreach(int i, SClients.keys())
    {
        if (!SClients[i].soc->isOpen())
            SClients.remove(i);
    }
}

void TcpChat_Server::server_stop()
{
    if (server_status)
    {
        foreach(int i, SClients.keys())
        {
            QTextStream os(SClients[i].soc);
            os.setAutoDetectUnicode(true);
            os << QDateTime::currentDateTime().toString() << "\n";
            SClients[i].soc->close();
            SClients.remove(i);
        }
        serv->close();
        qDebug() << QString::fromStdString("Server was stopped!");
        server_status = 0;
    }
}

void TcpChat_Server::slotReadClient()
{
    // ѕолучаем объект сокета, который вызвал данный слот
    QTcpSocket* clientSocket = (QTcpSocket*)sender();

    // ѕолучаем дескриптор, дл€ того, чтоб в случае закрыти€ сокета удалить его из карты
    int idusersocs = clientSocket->socketDescriptor();

    //тут обрабатываютс€ данные от сервера
    QDataStream in(clientSocket);

    //если считываем новый блок первые 2 байта это его размер
    if (SClients[idusersocs].size == 0)
    {
        //если пришло меньше 2 байт ждем пока будет 2 байта
        if (clientSocket->bytesAvailable() < (int)sizeof(quint16))
            return;

        //считываем размер (2 байта)
        quint16 ss;
        in >> ss;
        SClients[idusersocs].size = ss;
    }

    //ждем пока блок прийдет полностью
    if (clientSocket->bytesAvailable() < SClients[idusersocs].size)
        return;
    else
        SClients[idusersocs].size = 0;//можно принимать новый блок

    QString cmd;
    QString message;

    in >> cmd;
    in >> message;

    qDebug() << SClients[idusersocs].name << "::" << cmd << "::" << message;

    if (cmd == QString::fromStdString("AUTH"))
    {
        bool been = false;
        foreach(int i, SClients.keys())
        {
            if (SClients[i].name == message && !been)
                been = true;
        }

        QString ans;
        if (!been)
        {
            SClients[idusersocs].auth = true;
            SClients[idusersocs].name = message;
            ans = "YES";
            this->send_to_one("MESSA", SClients[idusersocs].soc, this->getMessages());
        }
        else
        {
            ans = "NO";
        }

        //резервируем 2 байта дл€ размера блока.
        this->send_to_one("AUTH", SClients[idusersocs].soc, ans);
    }
    else if (cmd == QString::fromStdString("MESS"))
    {
        if (SClients[idusersocs].auth)
        {
            message = SClients[idusersocs].name + QString::fromStdString("\n") + message;
            this->Messages.insert(0, message);
            this->send_to_all(cmd, message);
        }
    }

    emit s_update();
}

void TcpChat_Server::send_to_all(QString cmd, QString mess)
{
    if (this->server_status)
    {
        foreach(int i, SClients.keys())
            this->send_to_one(cmd, SClients[i].soc, mess);
    }
}

void TcpChat_Server::send_clients()
{
    if (this->server_status)
    {
        //from qmap to qlist
        QList<QString> addr;
        foreach(int i, SClients.keys())
            if (SClients[i].auth)
                addr.append(SClients[i].name);

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        //резервируем 2 байта дл€ размера блока.
        out << (quint16)0;
        out << QString::fromStdString("CLIENTS");
        out << addr;

        //возваращаемс€ в начало
        out.device()->seek(0);

        //вписываем размер блока на зарезервированное место
        out << (quint16)(block.size() - sizeof(quint16));

        foreach(int i, SClients.keys())
            SClients[i].soc->write(block);
    }
}

QList<QString> TcpChat_Server::getClients()const
{
    QList<QString> res;
    foreach(int i, SClients.keys())
        res.append(SClients[i].name + "(" + SClients[i].soc->peerAddress().toString() + ")");
    return res;
}
