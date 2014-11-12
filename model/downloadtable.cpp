#include "downloadtable.h"

DownloadTable::DownloadTable(QObject *parent) :
    QAbstractItemModel(parent)
{ }

QModelIndex DownloadTable::index(int row, int column, const QModelIndex &parent) const {
    (void)parent;
    return createIndex(row, column);
}

QModelIndex DownloadTable::parent(const QModelIndex &child) const {
    (void)child;
    return QModelIndex();
}

int DownloadTable::rowCount(const QModelIndex &parent) const {
    return 2;
}

int DownloadTable::columnCount(const QModelIndex &parent) const {
    return 2;
}

QVariant DownloadTable::data(const QModelIndex &index, int role) const {
    if(role == Qt::DisplayRole)
        return tr("test %1 %2").arg(index.row()).arg(index.column());
    else return QVariant();
}
