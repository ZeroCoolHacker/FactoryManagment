#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

SettingsDialog::SettingsDialog(QSqlDatabase *database,
                               QWidget *parent) :
    QDialog(parent),
    db(database),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    initializeModels();
    setupModels();
    ui->tabWidget->setCurrentIndex(0);
}

void SettingsDialog::initializeModels()
{
    user_model = new QSqlQueryModel(this);
    rawmaterial_model = new QSqlQueryModel(this);
    material_model = new QSqlQueryModel(this);
    employee_model = new QSqlQueryModel(this);
}

void SettingsDialog::setupModels()
{
    setupUsersTabModel();
    setupRawMaterialsTabModel();
    setupMaterialsTabModel();
    setupEmployeesTabModel();
}

void SettingsDialog::setupUsersTabModel()
{
    QSqlQuery q(*db);
    if(!q.exec("select * from users")){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
    }

    user_model->setQuery(q);

    ui->users_view->setModel(user_model);
}

void SettingsDialog::setupMaterialsTabModel()
{
    QSqlQuery q(*db);
    if(!q.exec("select * from materials")){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
    }

    material_model->setQuery(q);

    ui->sellingmaterial_view->setModel(material_model);
}

void SettingsDialog::setupRawMaterialsTabModel()
{
    QSqlQuery q(*db);
    if(!q.exec("select * from raw_materials")){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
    }

    rawmaterial_model->setQuery(q);

    ui->rawmaterials_view->setModel(rawmaterial_model);
}

void SettingsDialog::setupEmployeesTabModel()
{
    qDebug() <<"setupEmployeesTabModel called";
    QSqlQuery q(*db);
    if(!q.exec("select name,rank from employees")){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
    }
    employee_model->setQuery(q);
    ui->employees_tableview->setModel(employee_model);
}

bool SettingsDialog::doesRawMaterialExists(const QString &material)
{
    QSqlQuery q(*db);
    q.prepare("select * from raw_materials where type=?");
    q.bindValue(0,material);
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::warning(this,"Error","Coudn't check raw materials"
                                          "due to error : "+err.text());
        return true;
    }
    if(q.next())
        return true;
    return false;
}

bool SettingsDialog::doesUserExists(const QString &username)
{
    QSqlQuery q(*db);
    q.prepare("select * from users where username=?");
    q.bindValue(0,username);
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::warning(this,"Error","Coudn't check users"
                                          "due to error : "+err.text());
        return true;
    }
    if(q.next())
        return true;
    return false;
}

bool SettingsDialog::doesSellingMaterialExists(const QString &material)
{
    QSqlQuery q(*db);
    q.prepare("select * from materials where type=?");
    q.bindValue(0,material);
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::warning(this,"Error","Coudn't check materials"
                                          "due to error : "+err.text());
        return true;
    }
    if(q.next())
        return true;
    return false;
}

bool SettingsDialog::doesEmployeeExists(const QString &employee)
{
    QSqlQuery q(*db);
    q.prepare("select * from employees where name=?");
    q.bindValue(0,employee);
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::warning(this,"Error","Coudn't check employees"
                                          "due to error : "+err.text());
        return true;
    }
    if(q.next())
        return true;
    return false;
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_tabWidget_tabBarClicked(int index)
{
    Q_UNUSED(index);

    setupModels();
}

void SettingsDialog::on_addrawmaterial_button_clicked()
{
    ///check if the user has provided the right information
    // check for empty field
    if(ui->newrawmaterialtype_edit->text().isEmpty()){
        QMessageBox::warning(this,"No Type","Kindly fill in the type field");
        return;
    }
    //check if the material already exists
    if(doesRawMaterialExists(ui->newrawmaterialtype_edit->text())){
        QMessageBox::warning(this,"Exists","The material already exists in the list");
        return;
    }
    ///prepare the query

    QSqlQuery q(*db);
    q.prepare("insert into raw_materials (type,quantity) values(:type,:quantity)");
    q.bindValue(":type",ui->newrawmaterialtype_edit->text());
    q.bindValue(":quantity",ui->newrawmaterialspinBoxedit->value());

    //execute the query
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
        return;
    }
    setupRawMaterialsTabModel();
    ui->newrawmaterialtype_edit->clear();
    ui->newrawmaterialspinBoxedit->clear();
}

void SettingsDialog::on_adduser_button_clicked()
{
    ///check if the user has provided the right information
    // check for empty field
    if(ui->newusername_edit->text().isEmpty()){
        QMessageBox::warning(this,"Empty Fields","Kindly fill in the type field");
        return;
    }
    //check if the material already exists
    if(doesUserExists(ui->newusername_edit->text())){
        QMessageBox::warning(this,"Exists","The user already exists in the list");
        return;
    }
    ///prepare the query

    QSqlQuery q(*db);
    q.prepare("insert into users (username,password) values(:name,:pass)");
    q.bindValue(":name",ui->newusername_edit->text());
    q.bindValue(":pass",ui->newpassword_edit->text());

    //execute the query
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
        return;
    }
    setupUsersTabModel();
    ui->newusername_edit->clear();
}


void SettingsDialog::on_addsellingmaterial_button_clicked()
{
    ///check if the user has provided the right information
    // check for empty field
    if(ui->newsellingmaterialtype_edit->text().isEmpty()){
        QMessageBox::warning(this,"No Type","Kindly fill in the type field");
        return;
    }
    //check if the material already exists
    if(doesSellingMaterialExists(ui->newsellingmaterialtype_edit->text())){
        QMessageBox::warning(this,"Exists","The material already exists in the list");
        return;
    }
    ///prepare the query

    QSqlQuery q(*db);
    q.prepare("insert into materials (type,quantity) values(:type,:quantity)");
    q.bindValue(":type",ui->newsellingmaterialtype_edit->text());
    q.bindValue(":quantity",ui->newsellingmaterialspinBoxedit_2->value());

    //execute the query
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
        return;
    }
    setupMaterialsTabModel();
    ui->newsellingmaterialtype_edit->clear();
    ui->newsellingmaterialspinBoxedit_2->clear();
}

void SettingsDialog::on_addemployee_pushbutton_clicked()
{
    ///check if the user has provided the right information
    // check for empty field
    if(ui->newemployeename_edit->text().isEmpty()){
        QMessageBox::warning(this,"Empty Fields","Kindly fill in the name field");
        return;
    }
    //check if the employee already exists
    if(doesEmployeeExists(ui->newemployeename_edit->text())){
        QMessageBox::warning(this,"Exists","The employee already exists in the list");
        return;
    }
    ///prepare the query

    QSqlQuery q(*db);
    q.prepare("insert into employees (name,rank) values(:name,:rank)");
    q.bindValue(":name",ui->newemployeename_edit->text());
    q.bindValue(":rank","Machine Man");

    //execute the query
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error",err.text());
        return;
    }
    setupEmployeesTabModel();
    ui->newemployeename_edit->clear();
}
