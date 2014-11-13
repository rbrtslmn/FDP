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
    explicit DownloadTable(const net::DownloadManager *downloadManager, QObject *parent = 0);
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    static QString Speed2String(float Bps);
    static QString DownloadStatus2String(net::DownloadStatus status);

protected slots:
    void handleDownloadInformation(int downloadIdx, net::InformationType prop);

protected:
    const net::DownloadManager *downloadManager;

};

} // end of namespace model
} // end of namespace fdp

#endif // DOWNLOADTABLE_H
