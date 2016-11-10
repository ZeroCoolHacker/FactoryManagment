#include "paymentdialog.h"
#include "ui_paymentdialog.h"
#include <QMessageBox>
#include <QDebug>

PaymentDialog::PaymentDialog(QString dealer, QSqlDatabase *database,
                             bool recieving,QWidget *parent) :
    db(database),
    QDialog(parent),
    _recieving(recieving),
    ui(new Ui::PaymentDialog)
{
    ui->setupUi(this);

    //setup models
    ui->dateEdit->setDate(QDate::currentDate());
    ui->dealer_edit->setText(dealer);
    //set the debt
    if(_recieving)
        showDebt();
    else{
        showBalance();
        ui->recieve_button->setText("Pay");
    }
}

bool PaymentDialog::recievePayment()
{
    QString dealer = ui->dealer_edit->text();

    QString money = ui->money_spinBox->text();
    QString date = ui->dateEdit->date().toString("yyyy-MM-dd");

    qDebug()<<dealer;
    //prepare query
    QString nature;
    if(_recieving)
        nature = "Recieved";
    else
        nature = "Paid";
    QSqlQuery q(*db);
    q.prepare("insert into payments(dealer,money,date,time,nature)"
              " values(?,?,?,?,?)");
    q.bindValue(0,dealer);
    q.bindValue(1,money);
    q.bindValue(2,date);
    q.bindValue(3,QTime::currentTime().toString("hh:mm:AP"));
    q.bindValue(4,nature);

    //execute
    if(!(q.exec())){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
        return false;
    }
    if(!removeDebt(dealer,money))
        return false;
    return true;
}

bool PaymentDialog::givePayment()
{
    QString dealer = ui->dealer_edit->text();

    QString money = ui->money_spinBox->text();
    QString date = ui->dateEdit->date().toString("yyyy-MM-dd");
    QString nature;
    if(_recieving)
        nature = "Recieved";
    else
        nature = "Paid";

    qDebug()<<dealer;
    //prepare query
    QSqlQuery q(*db);
    q.prepare("insert into payments(dealer,money,date,time,nature)"
              " values(?,?,?,?,?)");
    q.bindValue(0,dealer);
    q.bindValue(1,money);
    q.bindValue(2,date);
        q.bindValue(3,QTime::currentTime().toString("hh:mm:AP"));
    q.bindValue(4,nature);

    //execute
    if(!(q.exec())){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
        return false;
    }
    if(!removeDebt(dealer,money))
        return false;
    return true;
}

bool PaymentDialog::removeDebt(QString dealer, QString money)
{
    QSqlQuery q(*db);

    if(_recieving)
        q.prepare("update dealers set debt = debt - ? where name = ?");
    else
        q.prepare("update supplier set balance = balance - ? where name = ?");

    q.bindValue(0,money);
    q.bindValue(1,dealer);

    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
        return false;
    }
    return true;
}

void PaymentDialog::showDebt()
{
    QSqlQuery q(*db);
    QString dealer = ui->dealer_edit->text();
    q.exec("select debt from dealers where name = '"+dealer+"'");
    if(q.next())
        ui->balance_label->setText(q.value("debt").toString());
}

void PaymentDialog::showBalance()
{
    QSqlQuery q(*db);
    QString supplier = ui->dealer_edit->text();
    q.exec("select balance from supplier where name = '"+supplier+"'");
    if(q.next())
        ui->balance_label->setText(q.value("balance").toString());
}

PaymentDialog::~PaymentDialog()
{
    delete ui;
}

void PaymentDialog::on_recieve_button_clicked()
{
    //confirms from the user
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Are You Sure?",
                                  "You can not undo the entry."
                                  "Do you want to continue?",
                                  QMessageBox::Yes | QMessageBox::Cancel);
    //if the user accepts the dialog
    if(reply==QMessageBox::Yes){
        //in case of pay
        if(_recieving){
            if(recievePayment()){
                QMessageBox::information(this,"Congratulations",
                                         "Payment Successfully recieved.");
                this->close();
            }
        }else{
            if(givePayment())
                QMessageBox::information(this,"Congratulations",
                                         "Payment Successfully paid.");
                this->close();
        }

    }
}

void PaymentDialog::on_money_spinBox_valueChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if(!(ui->dealer_edit->text().isEmpty())){
        ui->recieve_button->setEnabled(true);
    }
    else
        ui->recieve_button->setEnabled(false);
}
