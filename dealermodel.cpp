#include "dealermodel.h"
#include <QBrush>
#include <QColor>
#include <QModelIndex>
#include <QFont>

DealerModel::DealerModel(QObject *parent, short namecolumn,
                         short balancecoulumn) :
    QSqlQueryModel(parent),
    _namecol(namecolumn),
    _balancecol(balancecoulumn)
{

}

QVariant DealerModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && index.column()==_namecol
            && role==Qt::ForegroundRole){
        QColor text;
        text.setNamedColor("#3f9fd1");
        return QBrush(text);
    }
    if(index.isValid() && index.column()==_balancecol
            && role==Qt::ForegroundRole){
            QColor text;
            text.setNamedColor("#ba6000");
            return QBrush(text);
    }
    //font
    if(index.isValid() && role==Qt::FontRole){
        return QFont("times",12,QFont::DemiBold);
    }
    return QSqlQueryModel::data(index,role);
}

short DealerModel::balancecol() const
{
    return _balancecol;
}

void DealerModel::setBalancecol(short balancecol)
{
    _balancecol = balancecol;
}

short DealerModel::namecol() const
{
    return _namecol;
}

void DealerModel::setNamecol(short namecol)
{
    _namecol = namecol;
}
