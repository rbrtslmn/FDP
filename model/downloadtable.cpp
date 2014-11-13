#include "downloadtable.h"

namespace fdp {
namespace model {

DownloadTable::DownloadTable(const net::DownloadManager *downloadManager, QObject *parent) :
    QAbstractTableModel(parent),
    downloadManager(downloadManager)
{
    connect(downloadManager, SIGNAL(newInformation(int,net::InformationType)), this, SLOT(handleDownloadInformation(int,net::InformationType)));
}

void DownloadTable::handleDownloadInformation(int downloadIdx, net::InformationType prop) {
    (void)downloadIdx;
    (void)prop;

    // TODO: do not refresh the whole table

    beginResetModel();
    endResetModel();
}

int DownloadTable::rowCount(const QModelIndex &parent) const {
    (void)parent;
    return downloadManager->numberOfDownloads();
}

int DownloadTable::columnCount(const QModelIndex &parent) const {
    (void)parent;
    return 6;
}

QVariant DownloadTable::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
                case 0:
                    return tr("File");
                case 1:
                    return tr("Status");
                case 2:
                    return tr("Progress");
                case 3:
                    return tr("Speed");
                case 4:
                    return tr("URL");
                case 5:
                    return tr("Path");
            }
        }
    }
    return QVariant();
}

QVariant DownloadTable::data(const QModelIndex &index, int role) const {
    // invalid data
    if(!index.isValid())
        return QVariant();
    // display role
    if(role == Qt::DisplayRole) {
        switch(index.column()) {
        case 0:
            return downloadManager->downloadAt(index.row()).file;
        case 1:
            return DownloadStatus2String(downloadManager->downloadAt(index.row()).status);
        case 2:
            return tr("%1").arg(downloadManager->downloadAt(index.row()).progress);
        case 3:
            return Speed2String(downloadManager->downloadAt(index.row()).speed);
        case 4:
            return downloadManager->downloadAt(index.row()).url;
        case 5:
            return downloadManager->downloadAt(index.row()).path;
        }
    }
    return QVariant();
}

QString DownloadTable::DownloadStatus2String(net::DownloadStatus status) {
    switch(status) {
    case net::StatError:
        return "Error";
    case net::StatFinished:
        return "Finished";
    case net::StatInProgress:
        return "In Progress";
    case net::StatPending:
        return "Pending";
    }
    return "";
}

QString DownloadTable::Speed2String(float Bps) {
    int i=0;
    while(Bps >= 1000 && i<2) {
        Bps /= 1024;
        i++;
    }
    return tr("%1 %2%3").arg(Bps).arg(i<1?"":(i<2?"ki":"Mi")).arg("B/s");
}

} // end of namespace model
} // end of namespace fdp
