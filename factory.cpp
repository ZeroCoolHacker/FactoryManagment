#include "factory.h"
#include "ui_factory.h"
#include "selldialog.h"
#include "buydialog.h"
#include "paymentdialog.h"
#include "settingsdialog.h"
#include <QMessageBox>
#include <QDate>
#include <QModelIndexList>
#include <QModelIndex>
#include "reportsdialog.h"

Factory::Factory(QString user, QSqlDatabase *database, QWidget *parent) :
    username(user),
    db(database),
    QMainWindow(parent),
    ui(new Ui::Factory)
{
    ui->setupUi(this);
    //user information
    QString textlogin = username + " logged in";
    ui->user_label->setText(textlogin);

    //check database
    if(!db->open()){
        QMessageBox::critical(this,"Database","The database couldn't"
                                              "be connected.");
        ui->status_label->setText("Database not connected");
    }else
        ui->status_label->setText("Database connected");

    //setup models
    initializeModels();
    qDebug() << "Models initialized";
    setupModels();
    qDebug() << "Models setup completed";
}

void Factory::initializeModels()
{
    //initializes source models
    dealers_model = new DealerModel(this,0,1);
    deals_model = new SalesModel(this,0,1,3,5,6,7);
    payment_model = new PaymentModel(this);
    supplier_model = new DealerModel(this,0,1);
    supply_model = new SalesModel(this,0,1,2,3,4);
    stocks_model = new QSqlQueryModel(this);
    materials_model = new QSqlQueryModel(this);
    stockshift_model = new QStringListModel(this);
    machineman_model = new QSqlQueryModel(this);
    total_stock_model = new StockModel(this,1,500);

    //initializes proxy models
    dealer_proxy_model = new QSortFilterProxyModel(this);
    deals_proxy_model = new QSortFilterProxyModel(this);
    supplier_proxy_model = new QSortFilterProxyModel(this);
    supply_proxy_model = new QSortFilterProxyModel(this);
    payments_proxy_model = new QSortFilterProxyModel(this);

}

void Factory::setupModels()
{
    setupDealersTabModel();
    setupSuppliersTabModel();
    setupPaymentTabModel();
    setupDealsTabModel();
    setupStocksTabModel();
    setupTotalStockTabModel();
    setupSuppliesTabModel();
    setupStockShiftComboBoxModel();
    setupMaterialsModel();
    setupStockEmployeeComboBoxModel();
    //////////////////////////
    int month = QDate::currentDate().month();
    int year = QDate::currentDate().year();
    QDate currentmonth(year,month,1);
    //stock tab
    ui->stockview_dateedit->setDate(currentmonth);
    //payment tab
    ui->paymentdatesearch_dateEdit->setDate(QDate::currentDate().addMonths(-1));
    ui->paymentsdealersearch_lineEdit->setText(" ");
    ui->paymentsdealersearch_lineEdit->clear();
    ui->paymentsdealersearch_lineEdit->setFocus();
    //deals tab


    ui->dealsdatesearch_dateEdit->setDate(currentmonth);
    ui->dealsearch_edit->setText(" ");
    ui->dealsearch_edit->clear();
    //sales table modification
    ui->deals_tableview->resizeColumnToContents(0);
    //purchases tab modification
    ui->supplies_view->resizeColumnToContents(0);
    ui->suppliesdatesearch_dateEdit->setDate(currentmonth);
    //profit sales etc
    ui->fromserial_dateEdit->setDate(QDate::currentDate().addDays(-7));
    ui->toserial_dateEDit->setDate(QDate::currentDate());
    ui->toserial_dateEDit->setMaximumDate(QDate::currentDate());
    qDebug() << "ui setup complete";

}

void Factory::setupDealersTabModel()
{
    QSqlQuery q(*db);
    QString query = "select name,debt as balance from dealers";

    if(!q.exec(query)){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
    }
    dealers_model->setQuery(q);

    //set model
    ui->dealers_tableview->setModel(dealer_proxy_model);
    dealer_proxy_model->setSourceModel(dealers_model);
    qDebug() << "Dealers model setup";
}

void Factory::setupSuppliersTabModel()
{
    QSqlQuery q(*db);
    QString query = "select name,balance from supplier";

    if(!q.exec(query)){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
    }
    supplier_model->setQuery(q);

    //set model
    supplier_proxy_model->setSourceModel(supplier_model);
    ui->suppliers_tableview->setModel(supplier_proxy_model);
    qDebug() << "Supplier model setup";

}

void Factory::setupPaymentTabModel()
{
    QSqlQuery q(*db);
    q.prepare("select dealer as Name,money,date,time,nature from payments "
              "where date>?");
    q.bindValue(0,ui->paymentdatesearch_dateEdit->date().toString("yyyy-MM-dd"));

    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Payments",err.text());
    }
    payment_model->setQuery(q);
    payments_proxy_model->setSourceModel(payment_model);
    //set model
    ui->payment_tableview->setModel(payments_proxy_model);
    //view
    ui->payment_tableview->resizeColumnToContents(0);
    ui->payment_tableview->resizeColumnToContents(1);
    ui->payment_tableview->resizeColumnToContents(2);
    qDebug() << "Payment model setup";

}

void Factory::setupDealsTabModel()
{
    //get date
    int month = ui->dealsdatesearch_dateEdit->date().month();
    int year = QDate::currentDate().year();

    QDate dealsmonth(year,month,1);
    ui->dealsdatesearch_dateEdit->setDate(dealsmonth);
    QSqlQuery q(*db);
    q.prepare("select serial,dealer as Buyer,material,quantity,price_per_unit,total,"
                    "cash_paid,cash_remaining,date from deals where "
                    " date>?");
    q.bindValue(0,ui->dealsdatesearch_dateEdit->date().toString("yyyy-MM-dd"));

    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Deals Model",err.text());
    }
    deals_model->setQuery(q);

    //set model
    deals_proxy_model->setSourceModel(deals_model);
    ui->deals_tableview->setModel(deals_proxy_model);
    qDebug() << "Deals model setup";

}

void Factory::setupSuppliesTabModel()
{
    int month = ui->suppliesdatesearch_dateEdit->date().month();
    int year = QDate::currentDate().year();

    QDate suppliesmonth(year,month,1);
    ui->dealsdatesearch_dateEdit->setDate(suppliesmonth);
    QSqlQuery q(*db);
    q.prepare("select serial,supplier as Seller,"
              "net_weight,price,"
              "paid,date from supplies where date>?");
    q.bindValue(0,ui->suppliesdatesearch_dateEdit->date().toString("yyyy-MM-dd"));

    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Supplies",err.text());
    }
    supply_model->setQuery(q);

    //set model
    supply_proxy_model->setSourceModel(supply_model);
    ui->supplies_view->setModel(supply_proxy_model);
    ui->supplies_view->resizeColumnsToContents();
    ui->stackedWidget->setCurrentIndex(0);

    qDebug() << "Supplies model setup";

}

void Factory::setupStockShiftComboBoxModel()
{
  QStringList shifts;
  shifts << "Day" << "Night";
  stockshift_model->setStringList(shifts);
  ui->stockshift_comboBox->setModel(stockshift_model);
  ui->stockmaterial_comboBox->setCurrentIndex(0);
  ui->stockshift_comboBox->setCurrentIndex(0);
  qDebug() << "StockShiftComboBoxModel model setup";

}

void Factory::setupStockEmployeeComboBoxModel()
{
    QSqlQuery q(*db);
    q.prepare("select name from employees where rank=?");
    q.bindValue(0,"Machine Man");
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
    }
    machineman_model->setQuery(q);
    ui->stockmachineman_combobox->setModel(machineman_model);
    ui->stockmachineman_combobox->setCurrentIndex(0);
    qDebug() << "StockEmployeeComboBox model setup";

}

void Factory::setupMaterialsModel()
{
  QSqlQuery q(*db);
  QString query = "select type from materials";
  if(!q.exec(query)){
      QSqlError err = q.lastError();
      QMessageBox::critical(this,"Error",err.text());
  }
  materials_model->setQuery(q);
  ui->stockmaterial_comboBox->setModel(materials_model);
  ui->stockmaterial_comboBox->setCurrentIndex(1);
  qDebug() << "Materials model setup";

}

bool Factory::addStockShift()
{
    //check the fields
    if(ui->stockquantity_spinBox->value()==0){
        QMessageBox::information(this,"Can't Submit",
                                 "Fill in the quantity field to submit the shift"
                                 "details.");
        return false;
    }

    //gather info
    QString date = QDate::currentDate().toString("yyyy-MM-dd");
    QString shift = ui->stockshift_comboBox->currentText();
    QString material = ui->stockmaterial_comboBox->currentText();
    unsigned long long quantity = ui->stockquantity_spinBox->value();
    QString machineman = ui->stockmachineman_combobox->currentText();

    //prepare query
    QSqlQuery q(*db);
    q.prepare("INSERT INTO stocks(date,shift,machineman,material,quantity) "
              "VALUES (:date,:shift,:machineman,:material,:quantity)");
    q.bindValue(":date",date);
    q.bindValue(":shift",shift);
    q.bindValue(":machineman",machineman);
    q.bindValue(":material",material);
    q.bindValue(":quantity",quantity);
    //execute the query
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Shift Error","Couldn't save the shift"
                                                 "details.\n"+err.text());
        return false;
    }
    ///add the amount to total stock of material
    q.prepare("update materials set quantity=quantity+? where type=?");
    q.bindValue(0,quantity);
    q.bindValue(1,material);
    //execute the query
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Shift Error","Couldn't update the total"
                                                 "stock.\n"+err.text());
        return false;
    }
    ///everything goes well
    return true;
}

bool Factory::showSupplyDetail(QString serial)
{
    QSqlQuery q(*db);
    q.prepare("select detail from supplies where serial=?");
    q.bindValue(0,serial);
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::warning(this,"Sql",err.text());
        return false;
    }
    if(q.next()){
        QString detail = q.record().value("detail").toString();
        ui->supplydetial_label->setText(detail);
        return true;
    }
    else
        return false;
}

void Factory::setupStocksTabModel()
{
    int month = ui->stockview_dateedit->date().month();
    int year = QDate::currentDate().year();

    QDate stockmonth(year,month,1);
    ui->stockview_dateedit->setDate(stockmonth);
    QSqlQuery q(*db);
    q.prepare("select date,shift,material,quantity,machineman from stocks where date >?");
    q.bindValue(0,ui->stockview_dateedit->date().toString("yyyy-MM-dd"));

    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
    }

    //set model
    stocks_model->setQuery(q);
    ui->stocks_view->setModel(stocks_model);
    //view
    ui->stocks_view->resizeColumnToContents(0);
}

void Factory::setupTotalStockTabModel()
{
    QSqlQuery q(*db);
    q.prepare("select * from materials");

    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
    }

    //set model
    total_stock_model->setQuery(q);
    ui->totalstock_tableview->setModel(total_stock_model);
}

Factory::~Factory()
{
    delete ui;
}

void Factory::on_dealersearch_edit_textChanged(const QString &arg1)
{
    dealer_proxy_model->setFilterKeyColumn(0);
    dealer_proxy_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
    dealer_proxy_model->setFilterRegExp(arg1);
}

void Factory::on_suppliersearch_edit_textChanged(const QString &arg1)
{
    supplier_proxy_model->setFilterKeyColumn(0);
    supplier_proxy_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
    supplier_proxy_model->setFilterRegExp(arg1);
}

void Factory::on_dealsearch_edit_textChanged(const QString &arg1)
{
    deals_proxy_model->setFilterKeyColumn(1);
    deals_proxy_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
    deals_proxy_model->setFilterRegExp(arg1);
}

void Factory::on_TabView_tabBarClicked(int index)
{
    Q_UNUSED(index)
    setupModels();
}

void Factory::on_action_Sell_triggered()
{
    if(!ui->dealers_tableview->currentIndex().isValid())
        return;
    QModelIndexList indexes = ui->dealers_tableview->
            selectionModel()->selectedRows();
    if(!indexes.at(0).isValid())
        return;

    QString dealer = indexes.at(0).data().toString();
    SellDialog *sellDialog = new SellDialog(dealer,db,dealers_model,materials_model,this);
    sellDialog->setAttribute(Qt::WA_DeleteOnClose);
    sellDialog->exec();
}

void Factory::on_action_Buy_triggered()
{
    if(!ui->suppliers_tableview->currentIndex().isValid())
        return;
    QModelIndexList indexes = ui->suppliers_tableview->
            selectionModel()->selectedRows();
    if(!indexes.at(0).isValid())
        return;

    QString supplier = indexes.at(0).data().toString();
    BuyDialog *buyDialog = new BuyDialog(supplier,db,supplier_model,this);
    buyDialog->setAttribute(Qt::WA_DeleteOnClose);
    buyDialog->exec();
}

void Factory::on_actionRecieve_Payment_triggered()
{
    if(!ui->dealers_tableview->currentIndex().isValid())
        return;
    QModelIndexList indexes = ui->dealers_tableview->
            selectionModel()->selectedRows(0);
    if(!indexes.at(0).isValid())
        return;
    QString dealer;
    dealer =indexes.at(0).data().toString();
    PaymentDialog *paymentDialog = new PaymentDialog(dealer,db,this);
    paymentDialog->setAttribute(Qt::WA_DeleteOnClose);
    paymentDialog->exec();
}


void Factory::on_newdealername_edit_textChanged(const QString &arg1)
{
    ui->dealersearch_edit->setText(arg1);
}

void Factory::on_adddealer_button_clicked()
{
    if(ui->newdealername_edit->text().isEmpty()){
        QMessageBox::warning(this,"No Name","Please Enter a name");
        return;
    }
    // gather info
    QString name = ui->newdealername_edit->text();

    //prepare the query
    QSqlQuery q(*db);
    q.prepare("insert into dealers (name,debt) values (?,?)");
    q.bindValue(0,name);
    q.bindValue(1,0);

    //execute the query
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error Adding Dealer",err.text());
    }else{
        QMessageBox::information(this,"Dealer Added",name +" added to list.");
        setupDealersTabModel();
        ui->newdealername_edit->clear();
        ui->dealersearch_edit->clear();
    }
}


void Factory::on_newsuppliername_edit_textChanged(const QString &arg1)
{
    ui->suppliersearch_edit->setText(arg1);
}

void Factory::on_addsupplier_button_clicked()
{
    if(ui->newsuppliername_edit->text().isEmpty()){
        QMessageBox::warning(this,"No Name","Kindly enter a name.");
        return;
    }
    //gather info
    QString name = ui->newsuppliername_edit->text();

    //prepare the query
    QSqlQuery q(*db);
    q.prepare("insert into supplier (name,balance) values (?,?)");

    q.bindValue(0,name);
    q.bindValue(1,0);

    //execute the query
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error Adding Supplier",err.text());
    }else{
        QMessageBox::information(this,"Supplier Added",name +" added to list.");
        setupSuppliersTabModel();
        ui->newsuppliername_edit->clear();
        ui->suppliersearch_edit->clear();
    }
}

void Factory::on_supplysearch_edit_textChanged(const QString &arg1)
{
    supply_proxy_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
    supply_proxy_model->setFilterKeyColumn(0);
    supply_proxy_model->setFilterRegExp(arg1);
}


void Factory::on_actionOpen_Settings_triggered()
{
    SettingsDialog *settingsDialog = new SettingsDialog(db,this);
    settingsDialog->setAttribute(Qt::WA_DeleteOnClose);
    settingsDialog->exec();
}

void Factory::on_stockview_dateedit_userDateChanged(const QDate &date)
{
    Q_UNUSED(date)
    setupStocksTabModel();
}

void Factory::on_addstockshift_button_clicked()
{

    //confirms from the user
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Are You Sure?\n",
                                  "You can not undo the entry."
                                  "Do you want to continue?",
                                  QMessageBox::Yes | QMessageBox::Cancel);
    //if the user accepts the dialog
    if(reply == QMessageBox::Yes){
        ///begin transaction
        db->transaction();
        transacting = true;
        if(!addStockShift()){
            QMessageBox::warning(this,"Error","Could'n submit the request.\n"
                                              "try again!");
            db->rollback();
            transacting = false;
        }
        db->commit();
        transacting = false;
        ui->stockquantity_spinBox->clear();
        setupStocksTabModel();
        setupTotalStockTabModel();
    }
}

void Factory::on_dealers_tableview_doubleClicked(const QModelIndex &index)
{

    if(index.column()==0)
        on_action_Sell_triggered();
    else
        on_actionRecieve_Payment_triggered();
}

void Factory::on_suppliers_tableview_doubleClicked(const QModelIndex &index)
{
    if(index.column()==0)
        on_action_Buy_triggered();
    else
        on_actionPay_Bills_triggered();
}

void Factory::on_calendarWidget_clicked(const QDate &date)
{
    QDate d(date.year(),date.month(),1);
    ui->stockview_dateedit->setDate(d);
}

void Factory::on_pushButton_clicked()
{
    ui->calendarWidget->setSelectedDate(QDate::currentDate());
    ui->stockview_dateedit->setDate(QDate::currentDate());
}

void Factory::on_dealsdatesearch_dateEdit_dateChanged(const QDate &date)
{
    Q_UNUSED(date);
    setupDealsTabModel();
}



void Factory::on_supplyserialsearch_edit_textChanged(const QString &arg1)
{
    supply_proxy_model->setFilterKeyColumn(5);
    supply_proxy_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
    supply_proxy_model->setFilterRegExp(arg1);
}



void Factory::on_paymentdatesearch_dateEdit_dateChanged(const QDate &date)
{
    Q_UNUSED(date);
    setupPaymentTabModel();
}

void Factory::on_paymentsdealersearch_lineEdit_textChanged(const QString &arg1)
{
    payments_proxy_model->setFilterKeyColumn(0);
    payments_proxy_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
    payments_proxy_model->setFilterRegExp(arg1);
}

void Factory::on_generatereport_pushButton_clicked()
{
    //gather info
    QString fromdate = ui->fromserial_dateEdit->date().toString("yyyy-MM-dd");
    QString todate = ui->toserial_dateEDit->date().addDays(1).toString("yyyy-MM-dd");

    //prepare the query
    QSqlQuery q(*db);
    q.prepare("select sum(total),sum(cash_paid),sum(cash_remaining) from deals"
              " where date > :from and date < :to");
    q.bindValue(":from",fromdate);
    q.bindValue(":to",todate);

    //execute the query
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Report",err.text());
    }
    //display results
    if(q.next()){
        ui->salesreportsales_label->setText(q.value(0).toString());
        ui->salesreportrecieved_label->setText(q.value(1).toString());
        ui->salesreportbalance_label->setText(q.value(2).toString());
    }
    //total cash payments recieved apart from deals
    q.prepare("select sum(money) from payments where"
              " date>? and date<? and nature=?");
    q.bindValue(0,fromdate);
    q.bindValue(1,todate);
    q.bindValue(2,"Recieved");
    //execute the query
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Report",err.text());
    }
    if(q.next()){
        if(!q.value(0).toString().isEmpty())
            ui->salesreportpayments_lineEdit->setText(q.value(0).toString());
        else
            ui->salesreportpayments_lineEdit->setText("0");
    }
}

void Factory::on_actionPay_Bills_triggered()
{
    if(!ui->suppliers_tableview->currentIndex().isValid())
        return;
    if(!ui->suppliers_tableview->currentIndex().isValid())
        return;
    QModelIndexList indexes = ui->suppliers_tableview->
            selectionModel()->selectedRows(0);
    if(!indexes.at(0).isValid())
        return;
    QString supplier;
    supplier =indexes.at(0).data().toString();
    PaymentDialog *paymentDialog = new PaymentDialog(supplier,db,false,this);
    paymentDialog->setAttribute(Qt::WA_DeleteOnClose);
    paymentDialog->exec();
}

void Factory::on_supplies_view_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid())
            return;
    QModelIndexList indexes = ui->supplies_view->
            selectionModel()->selectedRows();
    if(!indexes.at(0).isValid())
        return;

    QString id = indexes.at(0).data().toString();
    if(!showSupplyDetail(id)){
        QMessageBox::warning(this,"Detail","Couldn't load the supply details"
                                           "page");
        return;
    }
    else
        ui->stackedWidget->setCurrentIndex(1);
}

void Factory::on_actionAbout_triggered()
{

}

void Factory::on_suppliesdatesearch_dateEdit_dateChanged(const QDate &date)
{
    Q_UNUSED(date);
    setupSuppliesTabModel();
}

void Factory::on_actionReports_triggered()
{
    ReportsDialog *dialog = new ReportsDialog(db,this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->exec();
}
