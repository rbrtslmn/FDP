#include "downloadtable.h"
#include "downloadtablecolumns.h"

#include <QPainter>

namespace fdp {
namespace model {

DownloadTable::DownloadTable(const net::DownloadManager *downloadManager, int progressColumnWidth, QObject *parent) :
    QAbstractTableModel(parent),
    downloadManager(downloadManager),
    progressColumnWidth(progressColumnWidth)
{ }

void DownloadTable::setProgressColumnWidth(int progressColumnWidth) {
    this->progressColumnWidth = progressColumnWidth;
}

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
                case ColumnFile:
                    return "File";
                case ColumnStatus:
                    return "Status";
                case ColumnSize:
                    return "Size";
                case ColumnProgress:
                    return "Progress";
                case ColumnSpeed:
                    return "Speed";
                case ColumnRemaining:
                    return "Remaining";
                case ColumnUrl:
                    return "URL";
                case ColumnPath:
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
    // handle roles
    switch(role) {
    case Qt::DisplayRole:
        return dataDisplayRole(index);
    case Qt::DecorationRole:
        return dataDecorationRole(index);
    case Qt::BackgroundRole:
        return dataBackgroundRole(index);
    case Qt::UserRole:
        return dataUserRole(index);
    }
    return QVariant();
}

QVariant DownloadTable::dataUserRole(const QModelIndex &index) const {
    switch(index.column()) {
    case ColumnFile:
        return downloadManager->downloadAt(index.row()).file;
    case ColumnStatus:
        return downloadManager->downloadAt(index.row()).status;
    case ColumnSize:
        return downloadManager->downloadAt(index.row()).size;
    case ColumnProgress:
        if(downloadManager->downloadAt(index.row()).size == 0)
            return 0;
        else
            return downloadManager->downloadAt(index.row()).progress/(qreal)downloadManager->downloadAt(index.row()).size;
    case ColumnSpeed:
        return downloadManager->downloadAt(index.row()).speed;
    case ColumnRemaining:
        return Remaining(
            downloadManager->downloadAt(index.row()).progress,
            downloadManager->downloadAt(index.row()).size,
            downloadManager->downloadAt(index.row()).speed);
    case ColumnUrl:
        return downloadManager->downloadAt(index.row()).url;
    case ColumnPath:
        return downloadManager->downloadAt(index.row()).path;
    }
    return QVariant();
}

QVariant DownloadTable::dataBackgroundRole(const QModelIndex &index) const {
    if(index.column() == ColumnProgress
    && progressColumnWidth != 0
    && downloadManager->downloadAt(index.row()).status == net::StatInProgress) {
        qreal val = downloadManager->downloadAt(index.row()).progress/(qreal)downloadManager->downloadAt(index.row()).size;
        if(val > 0) {
            QLinearGradient grad(0, 0, progressColumnWidth, 0);
            grad.setColorAt(val, interpolateProgressColor(index.row()));
            grad.setColorAt(val<0.9999?(val+0.0001):1, Qt::white);
            return QBrush(grad);
        }
    }
    return QVariant();
}

QVariant DownloadTable::dataDecorationRole(const QModelIndex &index) const {
    if(index.column() == ColumnStatus) {
        // error downloads
        if(downloadManager->downloadAt(index.row()).status == net::StatError
        || downloadManager->downloadAt(index.row()).status == net::StatFWError
        || downloadManager->downloadAt(index.row()).status == net::StatFileOffline
        || downloadManager->downloadAt(index.row()).status == net::StatLoginError
        || downloadManager->downloadAt(index.row()).status == net::StatInvalidUrl
        || downloadManager->downloadAt(index.row()).status == net::StatTimeout)
            return QVariant(QColor(255, 100, 100));
        // aborted downloads
        if(downloadManager->downloadAt(index.row()).status == net::StatAborted)
            return QVariant(QColor(255, 163, 100));
        // pending downloads
        if(downloadManager->downloadAt(index.row()).status == net::StatPending)
            return QVariant(QColor(100, 100, 255));
        if(downloadManager->downloadAt(index.row()).status == net::StatInProgress) {
            return QVariant(interpolateProgressColor(index.row()));
        }
        // finished downloads
        if(downloadManager->downloadAt(index.row()).status == net::StatFinished)
            return QVariant(QColor(100, 255, 100));
    }
    return QVariant();
}

QVariant DownloadTable::dataDisplayRole(const QModelIndex &index) const {
    switch(index.column()) {
    case ColumnFile:
        return downloadManager->downloadAt(index.row()).file;
    case ColumnStatus:
        return DownloadStatus2String(downloadManager->downloadAt(index.row()).status);
    case ColumnSize:
        if(downloadManager->downloadAt(index.row()).size != 0)
            return B2String(downloadManager->downloadAt(index.row()).size);
        else return "";
    case ColumnProgress:
        if(downloadManager->downloadAt(index.row()).size != 0)
            return tr("%1 %").arg((100 * downloadManager->downloadAt(index.row()).progress) / (float)downloadManager->downloadAt(index.row()).size, 0, '0', 2);
        return tr("0.00 %");
    case ColumnSpeed:
        if(downloadManager->downloadAt(index.row()).status == net::StatInProgress)
            return B2String(downloadManager->downloadAt(index.row()).speed).append("/s");
        return "";
    case ColumnRemaining:
        if(downloadManager->downloadAt(index.row()).status == net::StatInProgress)
            return Remaining(Remaining(downloadManager->downloadAt(index.row()).progress, downloadManager->downloadAt(index.row()).size, downloadManager->downloadAt(index.row()).speed));
        return "";
    case ColumnUrl:
        return downloadManager->downloadAt(index.row()).url;
    case ColumnPath:
        return downloadManager->downloadAt(index.row()).path;
    }
    return QVariant();
}

QColor DownloadTable::interpolateProgressColor(int idx) const {
    qreal val = downloadManager->downloadAt(idx).progress/(qreal)downloadManager->downloadAt(idx).size;
    int diff = val>0?(155 * val):0;
    return QColor(100, 100+diff, 255-diff);
}

float DownloadTable::Remaining(qint64 curr, qint64 size, float Bps) {
    return (size - curr)/Bps;
}

QString DownloadTable::Remaining(float sec) {
    int i = 0;
    while(sec >= 60 && i<2) {
        sec /= 60;
        i++;
    }
    return tr("%1 %2").arg(sec, 0, '0', 2).arg(i<1?"s":(i<2?"min":"h"));
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
        return "Aborted";
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
