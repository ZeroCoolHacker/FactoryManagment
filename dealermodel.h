#ifndef DEALERMODEL_H
#define DEALERMODEL_H

#include <QObject>
#include <QSqlQueryModel>
#include <QVariant>

class DealerModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit DealerModel(QObject *parent,short namecolumn=0,
                         short balancecoulumn=1);

    short namecol() const;
    void setNamecol(short namecol);

    short balancecol() const;
    void setBalancecol(short balancecol);

protected:
    QVariant data(const QModelIndex &item, int role) const;

private:
    short _namecol;
    short _balancecol;
};

#endif // DEALERMODEL_H
