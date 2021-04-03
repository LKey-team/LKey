#include "lkey.h"
#include "ui_lkey.h"

LKey::LKey(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LKey)
{
    ui->setupUi(this);

    // Подключение к серверу
    client = new QTcpSocket(this);
    client -> connectToHost("127.0.0.1", 33332);//192.168.43.189
    connect(client, SIGNAL(on_connected()), SLOT(slot_on_connected()));
    connect(client, SIGNAL(readyRead()), SLOT(slot_ready_read()));
    QHostAddress serverAddress = QHostAddress::LocalHost;

    // Получение текущего пути расположения файла
    pathS = pathS.substr(0, pathS.rfind("/"));
    pathQ = QString::fromStdString(pathS);

    // Установка фонового изображения
    bg.load(pathQ + "/interface/background.jpg");
    bg_w.load(pathQ + "/interface/background_w.jpg");
    bg = bg.scaled(this->size(), Qt::IgnoreAspectRatio);
    bg_w = bg_w.scaled(this->size(), Qt::IgnoreAspectRatio);
    palette.setBrush(QPalette::Background, bg);
    palette_w.setBrush(QPalette::Background, bg_w);
    this->setPalette(palette);

    // Добавление к ui-объектам CSS-стилей
    ui->title->setStyleSheet("QTextEdit { background-color: rgba(205, 214, 219, 0) }");
    ui->title_2->setStyleSheet("QTextEdit { background-color: rgba(205, 214, 219, 0) }");
    ui->text->setStyleSheet("QTextEdit { background-color: rgba(205, 214, 219, 0.8) }");
    ui->leftText->setStyleSheet("QTextEdit { background-color: rgba(205, 214, 219, 0.8) }");
    ui->rightText->setStyleSheet("QTextEdit { background-color: rgba(205, 214, 219, 0.8); font-family:'-apple-system','BlinkMacSystemFont','Segoe UI','Helvetica','Arial','sans-serif','Apple Color Emoji','Segoe UI Emoji'; font-size:22pt }");
    ui->file->setStyleSheet("QTextEdit { font-size:22pt; font-family:'-apple-system','BlinkMacSystemFont','Segoe UI','Helvetica','Arial','sans-serif','Apple Color Emoji','Segoe UI Emoji'; }");
    ui->authorization->setStyleSheet("QPushButton{background:rgba(205, 214, 219, 0.95); border-radius: 16px;}");
    ui->login->setStyleSheet("QLineEdit { padding-left: 10px; }");
    ui->text_ip->setStyleSheet("QLineEdit { padding-left: 10px; }");
    ui->password->setStyleSheet("QLineEdit { padding-left: 10px; }");
    ui->enter->setStyleSheet("QPushButton{background:rgba(205, 214, 219, 0.95); border-radius: 16px;}");
    ui->ok->setStyleSheet("QPushButton{background:rgba(205, 214, 219, 0.95); border-radius: 16px;}");
    ui->cancel->setStyleSheet("QPushButton{background:rgba(205, 214, 219, 0.95); border-radius: 16px;}");
    ui->back->setStyleSheet("QPushButton{background:rgba(205, 214, 219, 0.95); border-radius: 16px;}");
    ui->exit->setStyleSheet("QPushButton{background:rgba(205, 214, 219, 0.95); border-radius: 16px;}");
    ui->error->setStyleSheet("QTextEdit { background-color: rgba(205, 214, 219, 0.8) }");

    // Отключение видимости ui-объектов, не принадлежащих первому окну
    ui->title_2->setVisible(false);
    ui->login->setVisible(false);
    ui->password->setVisible(false);
    ui->enter->setVisible(false);
    ui->cancel->setVisible(false);
    ui->back->setVisible(false);
    ui->error->setVisible(false);
    ui->leftText->setVisible(false);
    ui->calendar->setVisible(false);
    ui->rightText->setVisible(false);
    ui->exit->setVisible(false);
    ui->file->setVisible(false);
    ui->text_ip->setVisible(false);
    ui->ok->setVisible(false);
}

LKey::~LKey()
{
    delete ui;
    client->close();
}

void LKey::authorize_step_1(QString login, QString password){
    // Формирование сообщения-авторизации для сервера
    QString message_to_server;
    message_to_server = "auth&" + login + "&" + password;

    // Отправка сообщения
    slot_send_to_sever(message_to_server);
}
void LKey::authorize_step_2(std::string message){
    // Действия в зависимости от ответа сервера на сообщение-авторизацию
    if (message == "true"){
            this->setPalette(palette);

            ui->title_2->setVisible(false);
            ui->login->setVisible(false);
            ui->password->setVisible(false);
            ui->enter->setVisible(false);
            ui->cancel->setVisible(false);
            ui->error->setVisible(false);

            ui->leftText->setVisible(true);
            ui->calendar->setVisible(true);
            ui->text_ip->setVisible(true);
            ui->ok->setVisible(true);
            ui->exit->setVisible(true);
        }
        else {
            this->setPalette(palette_w);

            ui->error->setVisible(true);
            ui->login->setText("");
            ui->password->setText("");
        }
}

void LKey::on_calendar_clicked(const QDate &date)
{
   // Получение выбранной пользователем даты-аудита
   f_data = date.toString("dd.MM.yy");
}
void LKey::on_ok_clicked()
{
    // Получение введенного пользователем ip-адреса
    QString ip = ui->text_ip->text();
    // Проверка заполнения всех полей
    if (f_data != "" && ip != ""){
        // Формирование сообщения-запроса на получение файла с сервера
        QString message_to_server;
        std::string date = f_data.toUtf8().constData();
        std::replace(date.begin(), date.end(), '.', '-');
        std::string s_ip = ip.toUtf8().constData();
        std::replace(s_ip.begin(), s_ip.end(), '.', '-');
        message_to_server = "file&" + QString::fromStdString(date) + "&" + QString::fromStdString(s_ip);

        // Отправка сообщения-запроса на сервер
        slot_send_to_sever(message_to_server);
    }

}

// Открытие файла полученного с сервера
void LKey::open_file(QString message){
    // Создание переменной файла
    QFile file(message);

    // Проверка файла на существование и открытие в режиме "Только чтение"
    if ((file.exists())&&(file.open(QIODevice::ReadOnly)))
    {
        // Заполнение ui-поля всей информацией полученной из файла
        ui->file->setText(file.readAll());
        file.close();

        // Настройка других ui-объектов, отображающихся в этом окне
        ui->rightText->setText("       Файл аудита за " + f_data + ":");

        ui->leftText->setVisible(false);
        ui->calendar->setVisible(false);
        ui->exit->setVisible(false);
        ui->text_ip->setVisible(false);
        ui->ok->setVisible(false);

        ui->file->setVisible(true);
        ui->rightText->setVisible(true);
        ui->back->setVisible(true);
    }
}


void LKey::on_authorization_clicked()
{
    // Настройка ui-объектов, отображающихся в следующем окне
    ui->title->setVisible(false);
    ui->text->setVisible(false);
    ui->authorization->setVisible(false);

    ui->title_2->setVisible(true);
    ui->login->setVisible(true);
    ui->password->setVisible(true);
    ui->enter->setVisible(true);
    ui->cancel->setVisible(true);
}

void LKey::on_cancel_clicked()
{
    // Смена фонового изображения
    this->setPalette(palette);

    // Настройка ui-объектов, отображающихся в следующем окне
    ui->title_2->setVisible(false);
    ui->login->setVisible(false);
    ui->password->setVisible(false);
    ui->enter->setVisible(false);
    ui->cancel->setVisible(false);
    ui->error->setVisible(false);

    ui->title->setVisible(true);
    ui->text->setVisible(true);
    ui->authorization->setVisible(true);

    ui->login->setText("");
    ui->password->setText("");
}

void LKey::on_enter_clicked()
{
    // Вызов функции авторизации
    authorize_step_1(ui -> login -> text(), ui -> password -> text());
}

void LKey::on_exit_clicked()
{
    // Настройка ui-объектов, отображающихся в следующем окне
    ui->leftText->setVisible(false);
    ui->calendar->setVisible(false);
    ui->rightText->setVisible(false);
    ui->exit->setVisible(false);
    ui->text_ip->setVisible(false);
    ui->ok->setVisible(false);

    ui->title->setVisible(true);
    ui->text->setVisible(true);
    ui->authorization->setVisible(true);

    ui->login->setText("");
    ui->password->setText("");
}

void LKey::on_back_clicked()
{
    // Настройка ui-объектов, отображающихся в следующем окне
    ui->file->setVisible(false);
    ui->rightText->setVisible(false);
    ui->back->setVisible(false);

    ui->leftText->setVisible(true);
    ui->calendar->setVisible(true);
    ui->text_ip->setVisible(true);
    ui->ok->setVisible(true);
    ui->exit->setVisible(true);
}


void LKey::slot_on_connected(){}

void LKey::slot_ready_read(){
    // Чтение и обработка сообщений, приходящих с сервера
    QByteArray array;
    std::string message = "";

    // Чтение
    while (client -> bytesAvailable() > 0){
        array = client -> readAll();
        message += array.toStdString();
    }

    // Обработка и вызов соответствующих функций
    if (message.substr(0, 4) == "auth"){
        message = message.substr(5);
        authorize_step_2(message);
    } else {
        message = message.substr(5);
        open_file(QString::fromStdString(message));
    }
}

void LKey::slot_send_to_sever(QString message){
    // Отправка сообщений на сервер
    QByteArray array;
    array.append(message);
    client -> write(array);
}

void LKey::slot_on_disconnected() {
    // Отключение клиента от сервера
    client -> close();
}
