#include <QCryptographicHash>

#include "downloadmanager.h"

namespace fdp {
namespace model {

DownloadManager::DownloadManager(int parallelDownloads) :
    QObject(),
    parallelDownloads(parallelDownloads)
{ }

void DownloadManager::setParallelDownloads(int parallelDownloads) {
    this->parallelDownloads = parallelDownloads;
    checkDownloads();
}

void DownloadManager::checkDownloads() {
    if(parallelDownloads < activeDownloads.length()) {
        // TODO: start new download
    }
}

void DownloadManager::addLink(QString username, QString password, QString path, QString url) {
    PendingDownload newDownload;
    newDownload.url = generatePremiumLink(username, password, url);
    newDownload.path = path;
    pendingDownloads.append(newDownload);
    checkDownloads();
}

QString DownloadManager::generatePremiumLink(QString username, QString password, QString url) {
    QString hash = generatePasswordHash(username, password);
    return QString("https://www.free-way.me/load.php?url=%1&user=%2&pw=%3&json")
        .arg(QString(url.toLatin1().toPercentEncoding()))
        .arg(username)
        .arg(hash);
}

QString DownloadManager::generatePasswordHash(QString username, QString password) {
    return QCryptographicHash::hash(username.toLower().append(password).toLatin1(), QCryptographicHash::Sha1);
}

} // end of namespace model
} // end of namespace fdp
