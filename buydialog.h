#ifndef BUYDIALOG_H
#define BUYDIALOG_H

#include <QDialog>
#include <QtSql>
#include <QStringList>
#include <QMessageBox>

namespace Ui {
class BuyDialog;
}

class BuyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BuyDialog(QString supplier,QSqlDatabase *database,
                       QSqlQueryModel *model,QWidget *parent = 0);

    void setupModels(QSqlQueryModel *model);
    void setupMaterialModel();
    void displayTotals();   //set the total values in line edits
    void showDetailString();    //creates the detatiled string of supply items
    bool buySupply();   //implements the buying logic of database
    bool updateSupplierBalance(QString supplier,qint64 remaining);  //adds the remaining amount to the suppliers account
    bool areFieldsEmpty();

    ~BuyDialog();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void on_next_toolbutton_clicked();
    void on_reset_button_clicked();
    void on_recievesupply_button_clicked();
    void on_weight_spinbox_valueChanged(int arg1);
    void on_price_spinbox_valueChanged(int arg1);

private:
    Ui::BuyDialog *ui;
    QSqlDatabase *db;
    QSqlQueryModel *material_model;
    qint64 net_weight;
    qint64 total_price;
    bool transacting =false;
};

#endif // BUYDIALOG_H
