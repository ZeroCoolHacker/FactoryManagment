#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QSqlDatabase *db, QWidget *parent = 0);
    void initializeModels();
    void setupModels();
    void setupUsersTabModel();
    void setupMaterialsTabModel();
    void setupRawMaterialsTabModel();
    void setupEmployeesTabModel();
    bool doesRawMaterialExists(const QString &material);
    bool doesUserExists(const QString &username);
    bool doesSellingMaterialExists(const QString &material);
    bool doesEmployeeExists(const QString &employee);
    ~SettingsDialog();

private slots:
    void on_tabWidget_tabBarClicked(int index);
    void on_addrawmaterial_button_clicked();
    void on_adduser_button_clicked();
    void on_addsellingmaterial_button_clicked();

    void on_addemployee_pushbutton_clicked();

private:
    Ui::SettingsDialog *ui;
    QSqlQueryModel *user_model;
    QSqlQueryModel *rawmaterial_model;
    QSqlQueryModel *material_model;
    QSqlQueryModel *employee_model;
    QSqlDatabase *db;
};

#endif // SETTINGSDIALOG_H
