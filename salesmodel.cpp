#include "salesmodel.h"

SalesModel::SalesModel(QObject *parent, int serial, int name,
                       int weight, int total, int paid, int remaining) :
    QSqlQueryModel(parent),
    _serial(serial),
    _name(name),
    _weight(weight),
    _total(total),
    _paid(paid),
    _remaining(remaining)
{

}

QVariant SalesModel::data(const QModelIndex &index, int role) const
{
    //serial no
    if(index.isValid() && index.column()==_serial && role==Qt::ForegroundRole){
        QColor text;
        text.setNamedColor("#ea4335");//google red
        return QBrush(text);
    }
    //name
    if(index.isValid() && index.column()==_name && role==Qt::ForegroundRole){
        QColor text;
        text.setNamedColor("#4285f4");//google blue
        return QBrush(text);
    }
    //paid price
    if(index.isValid() && index.column()==_paid && role==Qt::ForegroundRole){
        QColor text;
        text.setNamedColor("#ba6000");//qt orange
        return QBrush(text);
    }
    //total price
    if(index.isValid() && index.column()==_total && role==Qt::ForegroundRole){
        QColor text;
        text.setNamedColor("#34a853");//google green
        return QBrush(text);
    }
    //weight
    if(index.isValid() && index.column()==_paid && role==Qt::ForegroundRole){
        QColor text;
        text.setNamedColor("#ba6000");//qt orange
        return QBrush(text);
    }
    // cash remaining
    if(index.isValid() && index.column()==_remaining && role==Qt::ForegroundRole){
        QColor text;
        text.setNamedColor("#ea4335");//google red
        return QBrush(text);
    }
    //other color
    if(index.isValid() && role==Qt::ForegroundRole){
        QColor text;
        text.setNamedColor("#404244");//maingery
        return QBrush(text);
    }
    //font
    if(index.isValid() && role==Qt::FontRole){
        return QFont("times",12,QFont::DemiBold);
    }
    return QSqlQueryModel::data(index,role);
}

int SalesModel::weight() const
{
    return _weight;
}

void SalesModel::setWeight(int weight)
{
    _weight = weight;
}

int SalesModel::paid() const
{
    return _paid;
}

void SalesModel::setPaid(int paid)
{
    _paid = paid;
}

int SalesModel::total() const
{
    return _total;
}

void SalesModel::setTotal(int total)
{
    _total = total;
}

int SalesModel::name() const
{
    return _name;
}

void SalesModel::setName(int name)
{
    _name = name;
}



int SalesModel::serial() const
{
    return _serial;
}

void SalesModel::setSerial(int serial)
{
    _serial = serial;
}
