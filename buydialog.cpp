#include "buydialog.h"
#include "ui_buydialog.h"
#include <QKeyEvent>
#include <QEvent>

BuyDialog::BuyDialog(QString supplier, QSqlDatabase *database,
                     QSqlQueryModel *model, QWidget *parent) :
    db(database),
    QDialog(parent),
    net_weight(0),
    total_price(0),
    ui(new Ui::BuyDialog)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->price_spinbox->installEventFilter(this);


    //check database
    if(!db->open()){
        QMessageBox::critical(this,"Database","The database couldn't"
                                              "be connected.");
        ui->status_label->setText("Database not connected");
    }else
        ui->status_label->setText("Database connected");

    //update model
    setupModels(model);
    ui->supplier_edit->setText(supplier);
    ui->sr_spinbox->setFocus();

}

void BuyDialog::setupModels(QSqlQueryModel *model)
{
    Q_UNUSED(model);
    //setup combo boxes and list view
    setupMaterialModel();
}

void BuyDialog::setupMaterialModel()
{
    material_model = new QSqlQueryModel(this);
    QSqlQuery q(*db);
    QString query = "select type from raw_materials order by type";

    if(!q.exec(query)){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
    }
    material_model->setQuery(q);

    //set models of views
    ui->material_combo->setModel(material_model);
    ui->material_view->setModel(material_model);

    //set to the first element
    ui->material_combo->setCurrentIndex(0);
}

void BuyDialog::displayTotals()
{
    ui->netweight_spinbox->setValue(net_weight);
    ui->totalprice_spinbox->setValue(total_price);
}

void BuyDialog::showDetailString()
{
    QString material = ui->material_combo->currentText();
    QString weight = QString::number(ui->weight_spinbox->value());
    QString price = QString::number(ui->price_spinbox->value());

    QString supply = "<b><u><font face=\"verdana\" color=\"green\" size=\"5\">"
            +material+"</u></b></font><br>"
            +weight +" kg   |   "+price+" Rs<br>";
    //set the text edit
    QString old_text = ui->detail_text->toHtml();
    QString new_text = old_text+"<br>"+supply;
    ui->detail_text->setText(new_text);
}

bool BuyDialog::buySupply()
{
    QString serial_no = QString::number(ui->sr_spinbox->value());
    QString supplier = ui->supplier_edit->text();
    qint64 netWeight = ui->netweight_spinbox->value();
    qint64 price = ui->totalprice_spinbox->value();
    QString detail = ui->detail_text->toHtml();
    qint64 paid = ui->cashpaid_spinbox->value();
    QString date = ui->dateEdit->date().toString("yyyy-MM-dd");

    //calculate remaining cash
    qint64 remaining;
    if(price!=paid)
        remaining = price - paid;
    //prepare the query
    QSqlQuery q(*db);
    q.prepare("insert into supplies (serial,supplier,net_weight,price"
                    ",detail,paid,date) values (?,?,?,?,?,?,?)");
    q.bindValue(0,serial_no);
    q.bindValue(1,supplier);
    q.bindValue(2,netWeight);
    q.bindValue(3,price);
    q.bindValue(4,detail);
    q.bindValue(5,paid);
    q.bindValue(6,date);
    //execute the query
    if(!q.exec()){
            QSqlError err = q.lastError();
            QMessageBox::critical(this,"Error","Couldn't add supply info"
                                               "to the database\n"+err.text());
            return false;
    }else{
        //now add the remaining amount to the suppliers balance
        if(!updateSupplierBalance(supplier,remaining))
            return false;
    }
    return true;
}

bool BuyDialog::updateSupplierBalance(QString supplier, qint64 remaining)
{
    QSqlQuery q(*db);
    q.prepare("update supplier set balance =balance+? where name=?");
    q.bindValue(0,remaining);
    q.bindValue(1,supplier);
    //execute the query
    if(!q.exec()){
            QSqlError err = q.lastError();
            QMessageBox::critical(this,"Error","Couldn't update supplier"
                                               " balance"
                                               "due to "+err.text());
            return false;
    }else{
        QMessageBox::information(this,"Amount Added to "+supplier+"'s account.","Successfull");
        return true;
    }
}

bool BuyDialog::areFieldsEmpty()
{
    if(ui->weight_spinbox->value() == 0 ||
            ui->price_spinbox->value() == 0)
        return true;
    return false;
}

BuyDialog::~BuyDialog()
{
    delete material_model;
    delete ui;
}

bool BuyDialog::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==ui->price_spinbox && event->type()==QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->key()==Qt::Key_Return ||
                keyEvent->key()==Qt::Key_Enter){
            if(ui->next_toolbutton->isEnabled())
                on_next_toolbutton_clicked();
        }
    }
    return QDialog::eventFilter(watched,event);
}


/// private slots

void BuyDialog::on_next_toolbutton_clicked()
{
    //check if the fields are empty
    if(areFieldsEmpty()){
        qDebug() << "Fields empty";
            return;
    }

    //add to the total
    qint64 weight = ui->weight_spinbox->value();
    qint64 price = ui->calculatedprice_spinbox->value();
    total_price += price;
    net_weight += weight;
    displayTotals();
    //prepare the supply detail string
    showDetailString();
    //select the next material in combo box
    int count = ui->material_combo->count();
    int currentIndex = ui->material_combo->currentIndex();

    if(currentIndex!=(count-1)){
        ui->material_combo->setCurrentIndex(currentIndex+1);
        ui->weight_spinbox->setFocus();
    }

    //clear the fields
    ui->price_spinbox->clear();
    ui->weight_spinbox->clear();
    ui->next_toolbutton->setEnabled(false);
}


void BuyDialog::on_reset_button_clicked()
{
    net_weight = 0;
    total_price = 0;
    ui->weight_spinbox->clear();
    ui->price_spinbox->clear();
    ui->detail_text->clear();
    displayTotals();
}

void BuyDialog::on_recievesupply_button_clicked()
{
    //confirms from the user
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Are You Sure?",
                                  "You can not undo the entry."
                                  "Do you want to continue?",
                                  QMessageBox::Yes | QMessageBox::Cancel);
    //if the user accepts the dialog
    if(reply==QMessageBox::Yes){
        //begin the transaction
        db->transaction();
        transacting = true;
        //buy process started
        if(!buySupply()){
            QMessageBox::warning(this,"Failed","Failed to buy supply.try again!");
            db->rollback();
            transacting = false;
            return;
        }
        //if everything goes well
        db->commit();
        transacting = false;
        accept();
    }

}

void BuyDialog::on_weight_spinbox_valueChanged(int arg1)
{
    if(!(ui->weight_spinbox->value()<=0 ||
         ui->price_spinbox->value()<=0))
        ui->next_toolbutton->setEnabled(true);
    else
        ui->next_toolbutton->setEnabled(false);

    //calculate total price
    qint64 total = arg1*ui->price_spinbox->value();
    ui->calculatedprice_spinbox->setValue(total);
}

void BuyDialog::on_price_spinbox_valueChanged(int arg1)
{
    on_weight_spinbox_valueChanged(arg1);
    //calculate total price
    qint64 total = arg1*ui->weight_spinbox->value();
    ui->calculatedprice_spinbox->setValue(total);
}

