#ifndef LKEY_H
#define LKEY_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QHostAddress>

QT_BEGIN_NAMESPACE
namespace Ui { class LKey; }
QT_END_NAMESPACE

class LKey : public QMainWindow
{
    Q_OBJECT

public:
    LKey(QWidget *parent = nullptr);
    ~LKey();

private slots:
    void on_authorization_clicked();

    // Двухступенчатая авторизация
    void authorize_step_1(QString login, QString password);
    void authorize_step_2(std::string message);

    // Просмотр файлов-аудита
    void on_calendar_clicked(const QDate &date);
    void open_file(QString message);

    // Обработка нажатий кнопок
    void on_cancel_clicked();
    void on_enter_clicked();
    void on_exit_clicked();
    void on_back_clicked();
    void on_ok_clicked();

    // Слоты взаимодействия с сервером
    void slot_on_connected();
    void slot_ready_read();
    void slot_send_to_sever(QString message);
    void slot_on_disconnected();

private:
    Ui::LKey *ui;
    QPixmap bg, bg_w;
    QPalette palette, palette_w;

    QTcpSocket *client;

    QString f_data; // выбранная дата аудита

    // Получение текущего пути расположения файлов
    QString pathQ = QDir::currentPath();
    std::string pathS = pathQ.toUtf8().constData();
};
#endif // LKEY_H
