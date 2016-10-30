#ifndef SALESMODEL_H
#define SALESMODEL_H
#include <QObject>
#include <QSqlQueryModel>
#include <QVariant>
#include <QModelIndex>
#include <QColor>
#include <QFont>
#include <QBrush>

class SalesModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit SalesModel(QObject *parent=nullptr,int serial=0,int name=1,int weight=2,
                        int total=3,int paid=4,int remaining=5);

    int serial() const;
    void setSerial(int serial);

    int name() const;
    void setName(int name);

    int total() const;
    void setTotal(int total);

    int paid() const;
    void setPaid(int paid);

    int weight() const;
    void setWeight(int weight);

protected:
    QVariant data(const QModelIndex &index, int role) const;

private:
    int _serial;
    int _name;
    int _total;
    int _paid;
    int _weight;
    int _remaining;
};

#endif // SALESMODEL_H
