#ifndef DOWNLOADTABLE_H
#define DOWNLOADTABLE_H

#include <QAbstractItemModel>

class DownloadTable : public QAbstractItemModel {

    Q_OBJECT

public:
    explicit DownloadTable(QObject *parent = 0);
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

};

#endif // DOWNLOADTABLE_H
