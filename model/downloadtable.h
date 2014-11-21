#ifndef DOWNLOADTABLE_H
#define DOWNLOADTABLE_H

#include <QAbstractTableModel>
#include <QTimer>

#include <net/downloadmanager.h>

namespace fdp {
namespace model {

class DownloadTable : public QAbstractTableModel {

    Q_OBJECT

public:
    explicit DownloadTable(const net::DownloadManager *downloadManager, int progressColumnWidth = 0, QObject *parent = 0);
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    static QString Remaining(qint64 curr, qint64 size, float Bps);
    static QString B2String(float Bps);
    static QString DownloadStatus2String(net::DownloadStatus status);

    void beginInsert(int i);
    void endInsert();
    void beginDelete(int i);
    void endDelete();

public slots:
    void setProgressColumnWidth(int progressColumnWidth);

protected:
    QColor interpolateProgressColor(int idx) const;
    QVariant dataDisplayRole(const QModelIndex &index) const;
    QVariant dataDecorationRole(const QModelIndex &index) const;
    QVariant dataBackgroundRole(const QModelIndex &index) const;
    QVariant dataUserRole(const QModelIndex &index) const;



protected:
    const net::DownloadManager *downloadManager;
    int progressColumnWidth;

};

} // end of namespace model
} // end of namespace fdp

#endif // DOWNLOADTABLE_H
