#include "stockmodel.h"
#include <QModelIndex>
#include <QColor>
#include <QBrush>
#include <QFont>

StockModel::StockModel(QObject *parent, int quantitycolumn,
                       qint64 lowerlimit) :
    QSqlQueryModel(parent),
    _col(quantitycolumn),
    _lowerlimit(lowerlimit)
{

}

qint64 StockModel::lowerlimit() const
{
    return _lowerlimit;
}

void StockModel::setLowerlimit(const qint64 &lowerlimit)
{
    _lowerlimit = lowerlimit;
}

QVariant StockModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && index.column()==_col &&
            role==Qt::ForegroundRole){
        if(index.data().toLongLong() < _lowerlimit){
            QColor text;
            text.setNamedColor("#ea4335");//google red
            return QBrush(text);
        }else{
            QColor text;
            text.setNamedColor("#34a853");//google greeen
            return QBrush(text);
        }
    }
    //font
    if(index.isValid() && role==Qt::FontRole){
        return QFont("times",12,QFont::DemiBold);
    }
    //other colors of text
    if(index.isValid() && index.column()!=_col &&
        role==Qt::ForegroundRole){
        QColor text;
        text.setNamedColor("#4285f4");//google blue
        return QBrush(text);
    }
    return QSqlQueryModel::data(index,role);
}
