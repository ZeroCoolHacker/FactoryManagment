#ifndef PAYMENTDIALOG_H
#define PAYMENTDIALOG_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class PaymentDialog;
}

class PaymentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PaymentDialog(QString dealer, QSqlDatabase *db, bool recieving=true,
                           QWidget *parent = 0);
    bool recievePayment();
    bool givePayment();
    bool removeDebt(QString dealer,QString money);
    void showDebt();
    void showBalance();
    ~PaymentDialog();

private slots:
    void on_recieve_button_clicked();
    void on_money_spinBox_valueChanged(const QString &arg1);

private:
    Ui::PaymentDialog *ui;
    QSqlDatabase *db;
    bool _recieving;

};

#endif // PAYMENTDIALOG_H
