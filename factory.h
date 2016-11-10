#ifndef FACTORY_H
#define FACTORY_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QStringListModel>
#include "dealermodel.h"
#include "stockmodel.h"
#include "salesmodel.h"
#include "paymentmodel.h"

namespace Ui {
class Factory;
}

class Factory : public QMainWindow
{
    Q_OBJECT

public:
    explicit Factory(QString user,QSqlDatabase *database,QWidget *parent = 0);

    void initializeModels();
    void setupModels();
    void setupDealersTabModel();
    void setupSuppliersTabModel();
    void setupPaymentTabModel();
    void setupDealsTabModel();
    void setupStocksTabModel();
    void setupTotalStockTabModel();
    void setupSuppliesTabModel();
    void setupStockShiftComboBoxModel();
    void setupStockEmployeeComboBoxModel();
    void setupMaterialsModel();
    bool addStockShift();   //adds the details to stocks table
    bool showSupplyDetail(QString serial);

    ~Factory();

private slots:

    void on_dealersearch_edit_textChanged(const QString &arg1);
    void on_suppliersearch_edit_textChanged(const QString &arg1);
    void on_dealsearch_edit_textChanged(const QString &arg1);
    void on_TabView_tabBarClicked(int index);
    void on_action_Sell_triggered();
    void on_action_Buy_triggered();
    void on_actionRecieve_Payment_triggered();
    void on_newdealername_edit_textChanged(const QString &arg1);
    void on_adddealer_button_clicked();
    void on_newsuppliername_edit_textChanged(const QString &arg1);
    void on_addsupplier_button_clicked();
    void on_supplysearch_edit_textChanged(const QString &arg1);
    void on_actionOpen_Settings_triggered();

    void on_stockview_dateedit_userDateChanged(const QDate &date);

    void on_addstockshift_button_clicked();

    void on_dealers_tableview_doubleClicked(const QModelIndex &index);

    void on_suppliers_tableview_doubleClicked(const QModelIndex &index);

    void on_calendarWidget_clicked(const QDate &date);

    void on_pushButton_clicked();
    void on_dealsdatesearch_dateEdit_dateChanged(const QDate &date);

    void on_supplyserialsearch_edit_textChanged(const QString &arg1);


    void on_paymentdatesearch_dateEdit_dateChanged(const QDate &date);

    void on_paymentsdealersearch_lineEdit_textChanged(const QString &arg1);

    void on_generatereport_pushButton_clicked();

    void on_actionPay_Bills_triggered();

    void on_supplies_view_doubleClicked(const QModelIndex &index);

    void on_actionAbout_triggered();

    void on_suppliesdatesearch_dateEdit_dateChanged(const QDate &date);

    void on_actionReports_triggered();

private:
    Ui::Factory *ui;
    QString username;
    QSqlDatabase *db;
    //filters
    QSortFilterProxyModel *dealer_proxy_model;
    QSortFilterProxyModel *deals_proxy_model;
    QSortFilterProxyModel *supplier_proxy_model;
    QSortFilterProxyModel *supply_proxy_model;
    QSortFilterProxyModel *payments_proxy_model;
    //models
    DealerModel *dealers_model;
    SalesModel *deals_model;
    PaymentModel *payment_model;
    DealerModel *supplier_model;
    SalesModel *supply_model;
    QSqlQueryModel *stocks_model;
    QSqlQueryModel *materials_model;
    QStringListModel *stockshift_model;
    QSqlQueryModel *machineman_model;
    StockModel *total_stock_model;

    bool transacting = false;

};

#endif // FACTORY_H
