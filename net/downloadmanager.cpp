#include "downloadmanager.h"

namespace fdp {
namespace net {

DownloadManager::DownloadManager(int parallelDownloads, QObject *parent) :
    QObject(parent),
    parallelDownloads(parallelDownloads)
{ }

void DownloadManager::addLink(QString url, QString fwUrl, QString path) {
    DownloadInformation newDownload;
    newDownload.url = url;
    newDownload.fwUrl = fwUrl;
    newDownload.path = path;
    newDownload.status = StatPending;
    newDownload.progress = 0;
    newDownload.size = 0;
    newDownload.speed = 0;
    newDownload.downloader = new FWDownload();
    downloadList.append(newDownload);
    emit newInformation(downloadList.length() - 1, InfoNewDownload);
    checkDownloads();
}

void DownloadManager::checkDownloads() {
    int canStart = parallelDownloads - numberOfDownloads(StatInProgress);
    for(int i=0; i<downloadList.length() && canStart>0; i++) {
        if(downloadList.at(i).status == StatPending) {
            startDownload(downloadList.at(i));
            downloadList[i].status = StatInProgress;
            canStart--;
        }
    }
}

void DownloadManager::setParallelDownloads(int parallelDownloads) {
    this->parallelDownloads = parallelDownloads;
    checkDownloads();
}

void DownloadManager::startDownload(DownloadInformation downloadInformation) {
    connect(downloadInformation.downloader, SIGNAL(progress(qint64,qint64)), this, SLOT(handleDownloadProgress(qint64,qint64)));
    connect(downloadInformation.downloader, SIGNAL(speed(float)), this, SLOT(handleDownloadSpeed(float)));
    connect(downloadInformation.downloader, SIGNAL(error(QString)), this, SLOT(handleDownloadError(QString)));
    connect(downloadInformation.downloader, SIGNAL(finished()), this, SLOT(handleDownloadFinished()));
    connect(downloadInformation.downloader, SIGNAL(receivedFilename(QString)), this, SLOT(handleDownloadFilename(QString)));
    downloadInformation.downloader->start(downloadInformation.fwUrl, downloadInformation.path);
}

void DownloadManager::handleDownloadError(QString msg) {
    qDebug() << msg;
    for(int i=0; i<downloadList.length(); i++) {
        if(downloadList[i].downloader == sender()) {
            downloadList[i].downloader->disconnect();
            downloadList[i].error = msg;
            downloadList[i].speed = 0;
            downloadList[i].status = StatError;
            emit newInformation(i, InfoError);
            break;
        }
    }
    checkDownloads();
}

void DownloadManager::handleDownloadFinished() {
    for(int i=0; i<downloadList.length(); i++) {
        if(downloadList[i].downloader == sender()) {
            downloadList[i].downloader->disconnect();
            downloadList[i].speed = 0;
            downloadList[i].status = StatFinished;
            emit newInformation(i, InfoFinished);
            break;
        }
    }
    checkDownloads();
}

void DownloadManager::handleDownloadProgress(qint64 curr, qint64 size) {
    for(int i=0; i<downloadList.length(); i++) {
        if(downloadList[i].downloader == sender()) {
            downloadList[i].progress = curr;
            downloadList[i].size = size;
            emit newInformation(i, InfoProgress);
            emit newInformation(i, InfoSize);
            break;
        }
    }
}

void DownloadManager::handleDownloadSpeed(float Bps) {
    for(int i=0; i<downloadList.length(); i++) {
        if(downloadList[i].downloader == sender()) {
            downloadList[i].speed = Bps;
            emit newInformation(i, InfoSpeed);
            break;
        }
    }
}

void DownloadManager::handleDownloadFilename(QString filename) {
    for(int i=0; i<downloadList.length(); i++) {
        if(downloadList[i].downloader == sender()) {
            downloadList[i].file = filename;
            emit newInformation(i, InfoFilename);
            break;
        }
    }
}

DownloadInformation DownloadManager::downloadAt(int index) const {
    return downloadList[index];
}

int DownloadManager::numberOfDownloads() const {
    return downloadList.length();
}

int DownloadManager::numberOfDownloads(DownloadStatus status) const {
    int returnValue = 0;
    for(int i=0; i<downloadList.length(); i++) {
        if(downloadList.at(i).status == status) {
            returnValue++;
        }
    }
    return returnValue;
}

} // end of namespace net
} // end of namespace fdp
