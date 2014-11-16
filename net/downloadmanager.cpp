#include "downloadmanager.h"

#include <gui/reloadsettings.h>

#include "linkgenerator.h"

namespace fdp {
namespace net {

DownloadManager::DownloadManager(const int parallelDownloads, const int reloadSettings, const LoginData loginData, QObject *parent) :
    QObject(parent),
    parallelDownloads(parallelDownloads),
    reloadSettings(reloadSettings),
    loginData(loginData)
{
    timeoutTimer.start(1000);
    connect(&timeoutTimer, SIGNAL(timeout()), this, SLOT(check4Timeout()));
}

void DownloadManager::setReloadSettings(int reloadSettings) {
    this->reloadSettings = reloadSettings;
    checkDownloads();
}

void DownloadManager::check4Timeout() {
    for(int i=0; i<downloadList.length(); i++) {
        if(downloadList.at(i).status == StatInProgress) {
            if(downloadList.at(i).timeoutProgress != downloadList.at(i).progress) {
                downloadList[i].timeoutProgress = downloadList.at(i).progress;
                downloadList[i].timeoutCounter = 0;
            } else if(++downloadList[i].timeoutCounter >= 30) { // TODO: add this to the download settings
                downloadList[i].downloader->stop(true);
                downloadList[i].status = StatTimeout;
                emit newInformation(i, InfoState);
                checkDownloads();
            }
        }
    }
}

void DownloadManager::addLink(const QString url, const QString path) {
    DownloadInformation newDownload;
    newDownload.url = url;
    newDownload.path = path;
    if(!newDownload.path.endsWith("/"))
        newDownload.path += "/";
    newDownload.status = StatPending;
    newDownload.progress = 0;
    newDownload.size = 0;
    newDownload.speed = 0;
    newDownload.downloader = new FWDownload();
    newDownload.timeoutCounter = 0;
    downloadList.append(newDownload);
    emit newInformation(downloadList.length() - 1, InfoNewDownload);
    checkDownloads();
}

void DownloadManager::setLoginData(QString username, QString password) {
    loginData.username = username;
    loginData.password = password;
}

void DownloadManager::checkDownloads() {
    int canStart = parallelDownloads - numberOfDownloads(StatInProgress);
    for(int i=0; i<downloadList.length() && canStart>0; i++) {
        bool reloadThis = false;
        // reloading failed parts at occurence
        if((reloadSettings &  gui::ReloadAfterRest) == 0) {
            if(shouldReload(i))
                reloadThis = true;
        }
        // start pending
        if(downloadList.at(i).status == StatPending)
            reloadThis = true;
        if(reloadThis) {
            startDownload(downloadList.at(i));
            downloadList[i].status = StatInProgress;
            canStart--;
        }
    }
    // reloading failed parts when rest finished
    for(int i=0; i<downloadList.length() && canStart>0; i++) {
        if(reloadSettings & gui::ReloadAfterRest) {
            if(shouldReload(i)) {
                startDownload(downloadList.at(i));
                downloadList[i].status = StatInProgress;
                canStart--;
            }
        }
    }
}

bool DownloadManager::shouldReload(const int i) {
    return (downloadList.at(i).status == StatFWError && (reloadSettings & gui::ReloadFWError))
        || (downloadList.at(i).status == StatTimeout && (reloadSettings & gui::ReloadTimeout))
        || (downloadList.at(i).status == StatError   && (reloadSettings & gui::ReloadNetError));
}

void DownloadManager::setParallelDownloads(int parallelDownloads) {
    this->parallelDownloads = parallelDownloads;
    checkDownloads();
}

void DownloadManager::startDownload(const DownloadInformation downloadInformation) {
    connect(downloadInformation.downloader, SIGNAL(progress(qint64,qint64)), this, SLOT(handleDownloadProgress(qint64,qint64)));
    connect(downloadInformation.downloader, SIGNAL(speed(float)), this, SLOT(handleDownloadSpeed(float)));
    connect(downloadInformation.downloader, SIGNAL(error(QString)), this, SLOT(handleDownloadError(QString)));
    connect(downloadInformation.downloader, SIGNAL(finished()), this, SLOT(handleDownloadFinished()));
    connect(downloadInformation.downloader, SIGNAL(receivedFilename(QString)), this, SLOT(handleDownloadFilename(QString)));
    downloadInformation.downloader->start(
        LinkGenerator::GenerateFWLink(loginData.username, loginData.password, downloadInformation.url),
        downloadInformation.path);
}

void DownloadManager::handleDownloadError(QString msg) {
    qDebug() << msg;
    for(int i=0; i<downloadList.length(); i++) {
        if(downloadList[i].downloader == sender()) {
            downloadList[i].downloader->disconnect();
            downloadList[i].error = msg;
            downloadList[i].speed = 0;
            downloadList[i].status = StatError;
            emit newInformation(i, InfoState);
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
            // free-way.me error
            if(downloadList[i].file.startsWith("unnamed") && downloadList[i].size < 1024) {
                downloadList[i].status = StatFWError;
                QFile file(downloadList[i].path.append(downloadList[i].file));
                if(file.open(QIODevice::ReadOnly)) {
                    QString content = file.readAll();
                    file.close();
                    if(content.contains("File offline."))
                        downloadList[i].status = StatFileOffline;
                    if(content.contains("Ung&uuml;ltiger Login"))
                        downloadList[i].status = StatLoginError;
                }
            }
            emit newInformation(i, InfoState);
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

DownloadInformation DownloadManager::downloadAt(const int index) const {
    return downloadList[index];
}

int DownloadManager::numberOfDownloads() const {
    return downloadList.length();
}

int DownloadManager::numberOfDownloads(const DownloadStatus status) const {
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
