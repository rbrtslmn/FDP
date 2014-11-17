#include "downloadtable.h"

#include <QPainter>

namespace fdp {
namespace model {

DownloadTable::DownloadTable(const net::DownloadManager *downloadManager, QObject *parent) :
    QAbstractTableModel(parent),
    downloadManager(downloadManager)
{ }

void DownloadTable::beginInsert(int i) {
    beginInsertRows(QModelIndex(), i, i);
}

void DownloadTable::endInsert() {
    endInsertRows();
}

void DownloadTable::beginDelete(int i) {
    beginRemoveRows(QModelIndex(), i, i);
}

void DownloadTable::endDelete() {
    endRemoveRows();
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
    if(!index.isValid() || index.row() >= downloadManager->numberOfDownloads())
        return QVariant();
    // display role
    if(role == Qt::DisplayRole) {
        switch(index.column()) {
        case 0: // file
            return downloadManager->downloadAt(index.row()).file;
        case 1: // status
            return DownloadStatus2String(downloadManager->downloadAt(index.row()).status);
        case 2: // size
            if(downloadManager->downloadAt(index.row()).status == net::StatPending)
                return "";
            return B2String(downloadManager->downloadAt(index.row()).size);
        case 3: // progress
            if(downloadManager->downloadAt(index.row()).size != 0)
                return tr("%1 %").arg((100 * downloadManager->downloadAt(index.row()).progress) / (float)downloadManager->downloadAt(index.row()).size, 0, '0', 2);
            return tr("0.00 %");
        case 4: // speed
            if(downloadManager->downloadAt(index.row()).status == net::StatInProgress)
                return B2String(downloadManager->downloadAt(index.row()).speed).append("/s");
            return "";
        case 5: // remaining
            if(downloadManager->downloadAt(index.row()).status == net::StatInProgress)
                return Remaining(downloadManager->downloadAt(index.row()).progress, downloadManager->downloadAt(index.row()).size, downloadManager->downloadAt(index.row()).speed);
            return "";
        case 6: // url
            return downloadManager->downloadAt(index.row()).url;
        case 7: // path
            return downloadManager->downloadAt(index.row()).path;
        }
    } else if(role == Qt::BackgroundRole) {
        if(downloadManager->downloadAt(index.row()).status == net::StatError
        || downloadManager->downloadAt(index.row()).status == net::StatFWError
        || downloadManager->downloadAt(index.row()).status == net::StatFileOffline
        || downloadManager->downloadAt(index.row()).status == net::StatLoginError
        || downloadManager->downloadAt(index.row()).status == net::StatInvalidUrl
        || downloadManager->downloadAt(index.row()).status == net::StatTimeout)
            return QVariant(QColor(255, 100, 100));
        /*
        if(index.column() == 3 &&
        (downloadManager->downloadAt(index.row()).status == net::StatInProgress ||
         downloadManager->downloadAt(index.row()).status == net::StatFinished)) {
            qreal val = downloadManager->downloadAt(index.row()).progress/(qreal)downloadManager->downloadAt(index.row()).size;
            QLinearGradient grad(QPointF(0, 1), QPointF(1, 1));
            grad.setCoordinateMode(QGradient::ObjectBoundingMode);
            grad.setColorAt(val , QColor(100, 255, 100));
            grad.setColorAt(val+0.00001, Qt::white);
            return QBrush(grad);
        }
        */
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
    return tr("%1 %2").arg(s, 0, '0', 2).arg(i<1?"s":(i<2?"min":"h"));
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
    case net::StatFWError:
        return "Free-Way.me Error";
    case net::StatTimeout:
        return "Timeout";
    case net::StatFileOffline:
        return "File Offline";
    case net::StatLoginError:
        return "Wrong Login";
    case net::StatAborted:
        return "Download Aborted";
    case net::StatInvalidUrl:
        return "Invalid URL";
    }
    return "Unknown";
}

QString DownloadTable::B2String(float Bps) {
    int i=0;
    while(Bps >= 1000 && i<2) {
        Bps /= 1024;
        i++;
    }
    return tr("%1 %2B").arg(Bps, 0, '0', 2).arg(i<1?"":(i<2?"ki":"Mi"));
}

} // end of namespace model
} // end of namespace fdp
