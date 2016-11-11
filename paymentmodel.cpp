#include "paymentmodel.h"
#include <QDate>
#include <QBrush>

PaymentModel::PaymentModel(QObject *parent, int namecolumn,
                           int moneycolumn, int datecolumn, int timecolumn, int naturecolumn) :
    QSqlQueryModel(parent),
    _dealercolumn(namecolumn),
    _moneycolumn(moneycolumn),
    _datecolumn(datecolumn),
    _naturecolumn(naturecolumn)
{

}

QVariant PaymentModel::data(const QModelIndex &index, int role) const
{
    //name
    if(index.isValid() && index.column()==_dealercolumn &&
                           role==Qt::ForegroundRole){
        QColor text;
        text.setNamedColor("#3f9fd1");
        return QVariant::fromValue(QBrush(text));
    }
    //money
    if(index.isValid() && index.column()==_moneycolumn &&
            role==Qt::ForegroundRole){
        QColor text;
        text.setNamedColor("#34a853");
        return QVariant::fromValue(QBrush(text));
    }
    //date
    if(index.isValid() && index.column()==_datecolumn &&
            role==Qt::ForegroundRole){
        QColor text;
        text.setNamedColor("#ea4335");
        return QVariant::fromValue(QBrush(text));
    }
    //time
    if(index.isValid() && index.column()==_datecolumn &&
            role==Qt::ForegroundRole){
        QColor text;
        text.setNamedColor("#bcbebf");
        return QVariant::fromValue(QBrush(text));
    }
    //nature
    if(index.isValid() && index.column()==_datecolumn &&
            role==Qt::ForegroundRole){
        QColor text;
        text.setNamedColor("#bcbebf");
        return QVariant::fromValue(QBrush(text));
    }

    return QSqlQueryModel::data(index,role);
}
