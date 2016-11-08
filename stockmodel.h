#ifndef STOCKMODEL_H
#define STOCKMODEL_H

#include <QObject>
#include <QSqlQueryModel>
#include <QVariant>

class StockModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit StockModel(QObject *parent,int quantitycolumn=0,
                        qint64 lowerlimit=100);


    qint64 lowerlimit() const;
    void setLowerlimit(const qint64 &lowerlimit);

protected:
    QVariant data(const QModelIndex &index, int role) const;

private:
    qint64 _lowerlimit;
    int _col;
};

#endif // STOCKMODEL_H
