#ifndef PAYMENTMODEL_H
#define PAYMENTMODEL_H

#include <QObject>
#include <QSqlQueryModel>
#include <QVariant>
#include <QModelIndex>
#include <QColor>
#include <QFont>
#include <QString>

class PaymentModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    PaymentModel(QObject *parent=nullptr,int namecolumn=0,
                 int moneycolumn=1,int datecolumn=2,
                 int timecolumn=3,int naturecolumn=4);


protected:
    QVariant data(const QModelIndex &item, int role) const;

private:
    int _dealercolumn;
    int _moneycolumn;
    int _datecolumn;
    int _naturecolumn;
    int _timecolumn;
};

#endif // PAYMENTMODEL_H
