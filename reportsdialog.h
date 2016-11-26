#ifndef REPORTSDIALOG_H
#define REPORTSDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QPrinter>
#include <QPainter>
#include <QDebug>
#include "tableprinter.h"
#include <QPrintPreviewDialog>

namespace Ui {
class ReportsDialog;
}

class ReportsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReportsDialog(QSqlDatabase *db,QWidget *parent = 0);
    void initializeDateEdits();
    void initializeModels();
    void setupModels();
    void setupConnections();
    void setupDealerTabModel();
    void setupSupplierTabModel();
    void setupDrawingVariables();

    ~ReportsDialog();

public slots:
    void loadDealerPayments();
    void loadSupplierPayments();
    void dealerUglyPrint(QPrinter *printer);
    void supplierUglyPrint(QPrinter *printer);

private slots:
    void on_printDealerPayment_toolButton_clicked();
    void on_printsupplierpayments_toolbutton_clicked();

private:
    Ui::ReportsDialog *ui;
    QSqlDatabase *db;
    QSqlQueryModel *dealer_model;
    QSqlQueryModel *supplier_model;
    QSqlQueryModel *dealer_payments_model;
    QSqlQueryModel *supplier_payments_model;
    //for reports
    QVector<int> colStretch;
    QVector<QString> headers;
    QString _factoryName;
};

#endif // REPORTSDIALOG_H
