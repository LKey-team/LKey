#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <string>
#include <QString>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDir>

// Открытие базы данных
QSqlDatabase open_db();
// Функция авторизации
QString authorize(std::string message);
// Функция получения файла из базы данных
QString get_file(std::string message);
// Функция кейлоггера
QString keylogger(std::string message);

#endif // FUNCTIONS_H


/*
 * QString -> String: STR.toUtf8().constData()
 * String -> QString: QString::fromStdString(STR)
 */
