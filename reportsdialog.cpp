#include "reportsdialog.h"
#include "ui_reportsdialog.h"
#include <QMessageBox>
#include <QSqlError>
#include <QDateEdit>
#include <QFont>

// pageborderclass
class PrintBorder : public PagePrepare
{
public:
    virtual void preparePage(QPainter *painter);
    static int pageNumber;
};

int PrintBorder::pageNumber = 0;

void PrintBorder::preparePage(QPainter *painter) { // print a border on each page
    QRect rec = painter->viewport();
    painter->setPen(QPen(QColor(0, 0, 0), 1));
    painter->drawRect(rec);
    painter->translate(10, painter->viewport().height() - 10);
    painter->drawText(0, 0, QString("Page %1").arg(pageNumber));
    pageNumber += 1;
}
//<---------------------------------------------------------------


ReportsDialog::ReportsDialog(QSqlDatabase *database, QWidget *parent) :
    QDialog(parent),
    db(database),
    ui(new Ui::ReportsDialog)
{
    ui->setupUi(this);
    if(!db->isOpen()){
        if(!db->open())
            QMessageBox::critical(this,"Database","Database is not opened.");
        reject();
    }

    //initialization
    initializeDateEdits();
    setupConnections();
    initializeModels();
    setupModels();
    setupDrawingVariables();


}

void ReportsDialog::initializeDateEdits()
{
    //dealer tab
    ui->dealerfrom_dateEdit->setDate(QDate::currentDate().addDays(-7));
    ui->dealerto_dateEdit->setDate(QDate::currentDate());

    //supplier tab
    ui->supplierfrom_dateEdit_2->setDate(QDate::currentDate().addDays(-7));
    ui->supplierto_dateEdit_2->setDate(QDate::currentDate());

}

void ReportsDialog::initializeModels()
{
    dealer_model = new QSqlQueryModel(this);
    supplier_model = new QSqlQueryModel(this);
    dealer_payments_model = new QSqlQueryModel(this);
    supplier_payments_model = new QSqlQueryModel(this);
}

void ReportsDialog::setupModels()
{
    setupDealerTabModel();
    setupSupplierTabModel();
}

void ReportsDialog::setupConnections()
{
    //connections
    //dealer tab
    connect(ui->dealerto_dateEdit,&QDateEdit::dateChanged,
            this,&ReportsDialog::loadDealerPayments);
    connect(ui->dealerfrom_dateEdit,&QDateEdit::dateChanged,
            this,&ReportsDialog::loadDealerPayments);
    connect(ui->dealer_comboBox,&QComboBox::currentTextChanged,
            this,&ReportsDialog::loadDealerPayments);
    //supplier tab
    connect(ui->supplierto_dateEdit_2,&QDateEdit::dateChanged,
            this,&ReportsDialog::loadSupplierPayments);
    connect(ui->supplierfrom_dateEdit_2,&QDateEdit::dateChanged,
            this,&ReportsDialog::loadSupplierPayments);
    connect(ui->supplier_comboBox_2,&QComboBox::currentTextChanged,
            this,&ReportsDialog::loadSupplierPayments);
}

void ReportsDialog::setupDealerTabModel()
{
    /// Combo box
    // execute query
    QSqlQuery q(*db);
    q.prepare("select name from dealers");
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error","Could not load the"
                                           " dealers "+err.text());
        return;
    }
    dealer_model->setQuery(q);
    //set views
    ui->dealer_comboBox->setModel(dealer_model);

    //load payment details
    loadDealerPayments();
}

void ReportsDialog::setupSupplierTabModel()
{
    //execute queries
    QSqlQuery q(*db);
    q.prepare("select name from supplier");
    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error","Could not load the"
                                           " supplier "+err.text());
        return;
    }

    //set views
    supplier_model->setQuery(q);
    ui->supplier_comboBox_2->setModel(supplier_model);
    loadSupplierPayments();
}

void ReportsDialog::setupDrawingVariables()
{
    colStretch = QVector<int>() << 5 << 10 << 10 << 10 << 10 << 10;
    headers = QVector<QString>() << "ID" << "پارٹی" << "رقم" << "تاریخ"
                                                  << "وقت" << "Nature";
    _factoryName = "786 پلاسٹکس";
}

void ReportsDialog::loadDealerPayments()
{
    QSqlQuery q(*db);
    q.prepare("select serial,dealer,money,date,time,nature from payments"
              " where dealer=? and date>? and date<? and nature=?");
    q.bindValue(0,ui->dealer_comboBox->currentText());
    q.bindValue(1,ui->dealerfrom_dateEdit->date().addDays(-1).toString("yyyy-MM-dd"));
    q.bindValue(2,ui->dealerto_dateEdit->date().addDays(1).toString("yyyy-MM-dd"));
    q.bindValue(3,"Recieved");

    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error","Could not load the"
                                           " payments "+err.text());
        return;
    }
    //set view
    dealer_payments_model->setQuery(q);
    ui->dealer_tableview->setModel(dealer_payments_model);
}

void ReportsDialog::loadSupplierPayments()
{
    QSqlQuery q(*db);
    q.prepare("select * from payments"
              " where dealer=? and date>? and date<? and nature=?");
    q.bindValue(0,ui->supplier_comboBox_2->currentText());
    q.bindValue(1,ui->supplierfrom_dateEdit_2->date().addDays(-1).toString("yyyy-MM-dd"));
    q.bindValue(2,ui->supplierto_dateEdit_2->date().addDays(1).toString("yyyy-MM-dd"));
    q.bindValue(3,"Paid");

    if(!q.exec()){
        QSqlError err = q.lastError();
        QMessageBox::critical(this,"Error","Could not load the"
                                           " payments "+err.text());
        return;
    }
    //set view
    supplier_payments_model->setQuery(q);
    ui->supplier_tableView->setModel(supplier_payments_model);
}

void ReportsDialog::dealerUglyPrint(QPrinter *printer) {

    // ---------------- death-to-designers ------------------

    QPainter uglyPainter;
    if(!uglyPainter.begin(printer)) {
        qWarning() << "can't start printer";
        return;
    }
    TablePrinter uglyTablePrinter(&uglyPainter, printer);
    uglyTablePrinter.setPen(QPen(QColor(0, 0, 0), 3, Qt::SolidLine)); // pen for borders
    uglyTablePrinter.setHeaderColor(QColor(Qt::black));
    uglyTablePrinter.setContentColor(Qt::black);
    QFont font1; // font for headers
    font1.setPixelSize(16);
    font1.setBold(true);
    font1.setWeight(QFont::DemiBold);
    QFont font2; // font for content
    font2.setItalic(true);
    uglyTablePrinter.setHeadersFont(font1);
    uglyTablePrinter.setContentFont(font2);
    PrintBorder *printB = new PrintBorder;
    printB->pageNumber = 1;
    uglyTablePrinter.setPagePrepare(printB);
    uglyPainter.setPen(QPen(Qt::black));
    QFont titleFont;
    titleFont.setFamily("arial");
    titleFont.setWeight(QFont::Black);
    titleFont.setPixelSize(26);
    uglyPainter.setFont(titleFont);
    uglyPainter.drawText(uglyPainter.viewport().width()/2 - 40, 40, _factoryName);
    uglyPainter.translate(0, 60); // start print point
    uglyTablePrinter.setCellMargin(10, 5, 5, 5);
    uglyTablePrinter.setPageMargin(100, 40, 40, 40);
    if(!uglyTablePrinter.printTable(ui->dealer_tableview->model(), colStretch, headers)) {
        qDebug() << uglyTablePrinter.lastError();
    }
    uglyPainter.end();
    delete printB;
}

void ReportsDialog::supplierUglyPrint(QPrinter *printer)
{
    // ---------------- death-to-designers ------------------

    QPainter uglyPainter;
    if(!uglyPainter.begin(printer)) {
        qWarning() << "can't start printer";
        return;
    }
    TablePrinter uglyTablePrinter(&uglyPainter, printer);
    uglyTablePrinter.setPen(QPen(QColor(0, 0, 0), 3, Qt::SolidLine)); // pen for borders
    uglyTablePrinter.setHeaderColor(QColor(Qt::black));
    uglyTablePrinter.setContentColor(Qt::black);
    QFont font1; // font for headers
    font1.setBold(true);
    font1.setWeight(QFont::DemiBold);
    QFont font2; // font for content
    font2.setItalic(true);
    uglyTablePrinter.setHeadersFont(font1);
    uglyTablePrinter.setContentFont(font2);
    PrintBorder *printB = new PrintBorder;
    printB->pageNumber = 1;
    uglyTablePrinter.setPagePrepare(printB);
    uglyPainter.setPen(QPen(Qt::black));
    uglyPainter.drawText(uglyPainter.viewport().width()/2 - 40, 40, _factoryName);
    uglyPainter.translate(0, 60); // start print point
    uglyTablePrinter.setCellMargin(10, 5, 5, 5);
    uglyTablePrinter.setPageMargin(100, 40, 40, 40);
    if(!uglyTablePrinter.printTable(ui->supplier_tableView->model(), colStretch, headers)) {
        qDebug() << uglyTablePrinter.lastError();
    }
    uglyPainter.end();
    delete printB;
}

ReportsDialog::~ReportsDialog()
{
    delete ui;
}

void ReportsDialog::on_printDealerPayment_toolButton_clicked()
{
    QPrintPreviewDialog *dialog = new QPrintPreviewDialog;
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog,&QPrintPreviewDialog::paintRequested,
            this,&ReportsDialog::dealerUglyPrint);
    dialog->exec();
}

void ReportsDialog::on_printsupplierpayments_toolbutton_clicked()
{
    QPrintPreviewDialog *dialog = new QPrintPreviewDialog;
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog,&QPrintPreviewDialog::paintRequested,
            this,&ReportsDialog::supplierUglyPrint);
    dialog->exec();
}
