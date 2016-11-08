#ifndef SELLDIALOG_H
#define SELLDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QtSql>

namespace Ui {
class SellDialog;
}

class SellDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SellDialog(QString dealer, QSqlDatabase *database, QSqlQueryModel *dealers_model,
                        QSqlQueryModel *materials_model,QWidget *parent = 0);

    void setupModels(QSqlQueryModel *dealer, QSqlQueryModel *material);
    bool doDeal();
    void setupListModel(QSqlQueryModel *model);
    void setupComboboxModel(QSqlQueryModel *model);
    QString getTotalStock();
    bool updateStockAfterDeal(QString material, qint64 quantity);
    bool updateDebtAfterDeal(QString dealer, qint64 paid, qint64 total_price);
    bool isAnyFieldEmpty();

    ~SellDialog();

private slots:
    void on_toolButton_clicked();
    void on_dealers_view_clicked(const QModelIndex &index);
    void on_reset_button_clicked();
    void on_deal_button_clicked();
    void on_dealer_edit_textChanged(const QString &arg1);
    void on_material_combo_currentIndexChanged(const QString &arg1);

    void on_cost_spinbox_valueChanged(int arg1);

    void on_quantity_spinbox_valueChanged(int arg1);

private:
    Ui::SellDialog *ui;
    QSqlDatabase *db;
    bool transacting = false;
};

#endif // SELLDIALOG_H
