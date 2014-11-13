#include "downloadtable.h"

#include <QProgressBar>

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
    beginResetModel();
    endResetModel();
}

int DownloadTable::rowCount(const QModelIndex &parent) const {
    (void)parent;
    return downloadManager->numberOfDownloads();
}

int DownloadTable::columnCount(const QModelIndex &parent) const {
    (void)parent;
    return 8;
}

QVariant DownloadTable::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
                case 0:
                    return "File";
                case 1:
                    return "Status";
                case 2:
                    return "Size";
                case 3:
                    return "Progress";
                case 4:
                    return "Speed";
                case 5:
                    return "Remaining";
                case 6:
                    return "URL";
                case 7:
                    return "Path";
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
            return B2String(downloadManager->downloadAt(index.row()).size);
        case 3:
            return tr("%1 %").arg((100 * downloadManager->downloadAt(index.row()).progress) / (float)downloadManager->downloadAt(index.row()).size);
        case 4:
            return B2String(downloadManager->downloadAt(index.row()).speed).append("/s");
        case 5:
            return Remaining(downloadManager->downloadAt(index.row()).progress, downloadManager->downloadAt(index.row()).size, downloadManager->downloadAt(index.row()).speed);
        case 6:
            return downloadManager->downloadAt(index.row()).url;
        case 7:
            return downloadManager->downloadAt(index.row()).path;
        }
    }
    return QVariant();
}

QString DownloadTable::Remaining(qint64 curr, qint64 size, float Bps) {
    float s = (size - curr)/Bps;
    int i = 0;
    while(s >= 60 && i<2) {
        s /= 60;
        i++;
    }
    return tr("%1 %2").arg(s).arg(i<1?"s":(i<2?"min":"h"));
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

QString DownloadTable::B2String(float Bps) {
    int i=0;
    while(Bps >= 1000 && i<2) {
        Bps /= 1024;
        i++;
    }
    return tr("%1 %2B").arg(Bps).arg(i<1?"":(i<2?"ki":"Mi"));
}

} // end of namespace model
} // end of namespace fdp
