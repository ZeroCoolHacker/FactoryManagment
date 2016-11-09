#include "selldialog.h"
#include "ui_selldialog.h"

SellDialog::SellDialog(QString dealer, QSqlDatabase *database, QSqlQueryModel *dealers_model,
                       QSqlQueryModel *materials_model, QWidget *parent) :
    db(database),
    QDialog(parent),
    ui(new Ui::SellDialog)
{
    Q_ASSERT(dealers_model != Q_NULLPTR);
    Q_ASSERT(database != Q_NULLPTR);

    ui->setupUi(this);
    ui->dateTimeEdit->setDate(QDate::currentDate());

    //check database
    if(!db->open()){
        QMessageBox::critical(this,"Database","The database couldn't"
                                              "be connected.");
        ui->status_label->setText("Database not connected");
    }else
        ui->status_label->setText("Database connected");

    //update model
    setupModels(dealers_model,materials_model);
    ui->dealer_edit->setText(dealer);
    //set the debt
    QSqlQuery q(*db);
    q.prepare("select debt from dealers where name=?");
    q.bindValue(0,dealer);
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::warning(this,"Balance",
                             "Couldn't load balance due to "+ err.text());
        return;
    }
    if(q.next())
        ui->debt_label->setText(q.value("debt").toString());
}

void SellDialog::setupModels(QSqlQueryModel *dealer,QSqlQueryModel *material)
{
    //set DateTime
    ui->dateTimeEdit->setDate(QDate::currentDate());
    //update statistics
    QString total_stock = QString(getTotalStock());
    ui->totalstock_edit->setText(total_stock);
    //list view model/////////////////////////////
    setupListModel(dealer);
    //combo box model/////////////////////////////
    setupComboboxModel(material);
}

bool SellDialog::doDeal()
{
    //start transaction
    transacting = true;
    db->transaction();
    //prepare items for query
    QString dealer = ui->dealer_edit->text();
    QString material = ui->material_combo->currentText();
    qint64 quantity = ui->quantity_spinbox->value();
    qint64 price_per_unit = ui->cost_spinbox->value();
    qint64 total = ui->total_spinbox->value();
    qint64 cash_paid = ui->cashpaid_spinbox->value();
    QString date = ui->dateTimeEdit->date().toString("yyyy-MM-dd");
    //calculate remaining cash
    qint64 remaining = total - cash_paid;

    QSqlQuery q(*db);
    //prepare the query
    q.prepare("insert into deals(dealer,material,price_per_unit,"
                    "cash_paid,date,quantity,cash_remaining,total) "
                    "values(:dealer,:material,:price_per_unit,"
                    ":cash_paid,:date,:quantity,:cash_remaining,:total)");
    q.bindValue(":dealer",dealer);
    q.bindValue(":material",material);
    q.bindValue(":price_per_unit",price_per_unit);
    q.bindValue(":cash_paid",cash_paid);
    q.bindValue(":date",date);
    q.bindValue(":quantity",quantity);
    q.bindValue(":cash_remaining",remaining);
    q.bindValue(":total",total);

    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
        db->rollback();
        return false;
    }

    //calculate the stock left
    if(!updateStockAfterDeal(material,quantity)){
        db->rollback();
        transacting=false;
        return false;
    }
    //calculate debt
    if(!updateDebtAfterDeal(dealer,cash_paid,total)){
        db->rollback();
        transacting=false;
        return false;
    }

    //everything goes well then commit
    db->commit();
    transacting=false;
    return true;
}

void SellDialog::setupListModel(QSqlQueryModel *model)
{
    QSqlQuery q(*db);
    QString query = "select name from dealers order by name";

    //execute the query
    if(!q.exec(query)){
        QSqlError err = q.lastError();
        QMessageBox::warning(this,"Dealer List","The query for dealers"
                                                "couldn't be executed.Error :"
                                                " "+err.text());
    }

    //set model
    model->setQuery(q);
    ui->dealers_view->setModel(model);
}

void SellDialog::setupComboboxModel(QSqlQueryModel *model)
{
    QSqlQuery q(*db);
    //execute the query
    if(!q.exec("select type from materials order by type")){
        QSqlError err = q.lastError();
        QMessageBox::warning(this,"Material List",err.text());
    }
    //set the model
    model->setQuery(q);
    ui->material_combo->setModel(model);
}

QString SellDialog::getTotalStock()
{
    QSqlQuery q(*db);
    QString material=ui->material_combo->currentText();
    q.prepare("select quantity from materials where type=?");
    q.bindValue(0,material);
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::warning(this,"Total Stock",err.text());
        return "Error";
    }
    QString stock;
    if(q.next())
        stock = q.value("quantity").toString();
    return stock;
}

bool SellDialog::updateStockAfterDeal(QString material,qint64 quantity)
{
    QSqlQuery q(*db);
    q.prepare("update materials set quantity=quantity-? where type=?");
    q.bindValue(0,quantity);
    q.bindValue(1,material);
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error","Could't update stock .\n"+err.text());
        return false;
    }
    return true;
}

bool SellDialog::updateDebtAfterDeal(QString dealer, qint64 paid, qint64 total)
{
    if(paid != total){
        qint64 debt = total - paid;
        QSqlQuery q(*db);
        q.prepare("update dealers set debt=debt+? where name=?");
        q.bindValue(0,debt);
        q.bindValue(1,dealer);
        if(!q.exec()){
            QSqlError err = q.lastError();
            QMessageBox::critical(this,"Error","Could't update balance .\n"+err.text());
            return false;
        }
        return true;
    }
    return true;
}

bool SellDialog::isAnyFieldEmpty()
{
    if(ui->dealer_edit->text().isEmpty() ||
            ui->material_combo->currentText().isEmpty() ||
            ui->total_spinbox->value()<1 ||
            ui->total_spinbox->text().isEmpty() ||
            ui->cashpaid_spinbox->text().isEmpty() ||
            ui->cost_spinbox->text().isEmpty() ||
            ui->quantity_spinbox->value()<1 ||
            ui->quantity_spinbox->text().isEmpty())
        return true;
    return false;
}


SellDialog::~SellDialog()
{
    delete ui;
}

void SellDialog::on_toolButton_clicked()
{
    if(ui->total_spinbox->isReadOnly())
        ui->total_spinbox->setReadOnly(false);
    else
        ui->total_spinbox->setReadOnly(true);
}

void SellDialog::on_dealers_view_clicked(const QModelIndex &index)
{
    QString dealer_name = index.data().toString();
    ui->dealer_edit->setText(dealer_name);

    //set the debt
    QSqlQuery q(*db);
    q.exec("select debt from dealers where name = '"+dealer_name+"'");
    if(q.next())
        ui->debt_label->setText(q.value("debt").toString());
}

void SellDialog::on_reset_button_clicked()
{
    ui->dealer_edit->clear();
    ui->quantity_spinbox->clear();
    ui->cost_spinbox->clear();
    ui->total_spinbox->clear();
    ui->cashpaid_spinbox->clear();
    ui->total_spinbox->setEnabled(false);
    ui->totalstock_edit->setText(getTotalStock());
}

void SellDialog::on_deal_button_clicked()
{
    //check fields
    if(isAnyFieldEmpty()){
        QMessageBox::warning(this,"Empty Fields",
                             "Please Fill in the empty fields");
        return;
    }

    //confirms from the user
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Are You Sure?",
                                  "You can not undo the entry."
                                  "Do you want to continue?",
                                  QMessageBox::Yes | QMessageBox::Cancel);
    //if the user accepts the dialog
    if(reply == QMessageBox::Yes){
        if(doDeal())
                QMessageBox::information(this,"Done","Deal successfully done");
        else
            return;
        accept();
    }
    //else if he denies the dialog
    else if(reply==QMessageBox::Cancel){
        return;
    }
}

void SellDialog::on_dealer_edit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);

    if(ui->dealer_edit->text().isEmpty())
        ui->deal_button->setEnabled(false);
    else
        ui->deal_button->setEnabled(true);
}

void SellDialog::on_material_combo_currentIndexChanged(const QString &arg1)
{
    QString material = arg1;
    QSqlQuery q(*db);
    q.prepare("select quantity from materials where type=?");
    q.bindValue(0,material);
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
        return;
    }else{
        q.next();
        qint64 stock= q.value("quantity").toLongLong();
        ui->totalstock_edit->setText(QString::number(stock));
        ui->quantity_spinbox->setMaximum(stock);
    }
}

void SellDialog::on_cost_spinbox_valueChanged(int arg1)
{
    qint64 quantity = ui->quantity_spinbox->value();
    qint64 total = arg1 * quantity;
    ui->total_spinbox->setValue(total);
}

void SellDialog::on_quantity_spinbox_valueChanged(int arg1)
{
    qint64 cost_per_unit = ui->cost_spinbox->value();
    qint64  total= cost_per_unit * arg1;
    ui->total_spinbox->setValue(total);

    //check total stock
    int total_stock = ui->totalstock_edit->text().toInt();
    if(arg1>total_stock){
        ui->deal_button->setEnabled(false);
    }else
        ui->deal_button->setEnabled(true);
}
