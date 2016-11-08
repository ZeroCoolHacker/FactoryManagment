#include "logindialog.h"
#include "ui_logindialog.h"
#include "factory.h"
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

//    if(checkLicence())
       setupConnection();
}

bool LoginDialog::checkLicence()
{
    QString filepath = "C:/Windows/shaprianz.spz";
    QFile file(filepath);
    if(!file.exists()){
        QMessageBox::information(this,"Caution!",
                                 "<b>You are using this application "
                                 "without buying it.We can sue for "
                                 "5 lac rupees.Kindly contact the "
                                 "developer at <u>03316296093</u> to get "
                                 "your own software.");
        return false;
    }
    return true;
}

void LoginDialog::setupConnection()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    QString path = qApp->applicationDirPath()+"/Records/database.db";
    QFile dbFile(path);
    if(!dbFile.exists()){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,"Database","No database Exists."
                                              "Do you want to open your own database?",
                              QMessageBox::Yes | QMessageBox::No);
        if(reply==QMessageBox::Yes)
            path = QFileDialog::getOpenFileName(this,"Database",QDir::currentPath());
        else{
            qApp->closeAllWindows();
            qApp->exit(0);
        }
    }
    db.setDatabaseName(path);
    qDebug() << path;
    if(db.open())
        ui->status->setText("Database Connected");
    else
        ui->status->setText("Database not connected");
}

void LoginDialog::login()
{
    QString username = ui->username_edit->text();
    Factory *factory = new Factory(username,&db);
    factory->show();
    factory->showMaximized();
    this->hide();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_pushButton_clicked()
{
    QString username = ui->username_edit->text();
    QString password = ui->password_edit->text();

    QSqlQuery q(db);
    q.prepare("select * from users where username=? and password=?");
    q.bindValue(0,username);
    q.bindValue(1,password);
    if(q.exec())
        if(q.next()){
            int field = q.record().indexOf("username");
            ui->status->setText(q.value(field).toString()+" Logged in");
            login();
        }
        else
            ui->status->setText(username + " not found");
    else{
        ui->status->setText("Not logged in");
        QSqlError err = q.lastError();
        QMessageBox::warning(this,"Error",err.text());
    }
}
