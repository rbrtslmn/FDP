#include "downloadmanager.h"

#include <gui/reloadsettings.h>

#include "linkgenerator.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QApplication>
#include <QTextDocument>

namespace fdp {
namespace net {

DownloadManager::DownloadManager(const int parallelDownloads, const int reloadSettings, const LoginData loginData, QObject *parent) :
    QObject(parent),
    parallelDownloads(parallelDownloads),
    reloadSettings(reloadSettings),
    loginData(loginData)
{
    loadDownloads();
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
            } else {
                downloadList[i].speed = 0;
                if(++downloadList[i].timeoutCounter >= 30) { // TODO: add this to the download settings
                    downloadList[i].downloader->stop(true);
                    downloadList[i].status = StatTimeout;
                    checkDownloads();
                }
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
    checkDownloads();
}

void DownloadManager::setLoginData(QString username, QString password) {
    loginData.username = username;
    loginData.password = password;
}

void DownloadManager::checkDownloads() {
    int canStart = parallelDownloads - numberOfDownloads(StatInProgress);
    for(int i=0; i<downloadList.length() && canStart>0; i++) {
        bool loadThis = false;
        // normal pending file
        if(downloadList.at(i).status == StatPending) {
            loadThis = true;
        // reload failed parts at occurence
        } else if(!(reloadSettings &  gui::ReloadAfterRest) && shouldReload(i)) {
            loadThis = true;
        // reload failed parts when rest finished
        } else if(numberOfDownloads(StatPending) == 0 && shouldReload(i)) {
            loadThis = true;
        }
        if(loadThis) {
            resetDownloadData(i);
            downloadList[i].status = StatInProgress;
            startDownload(downloadList.at(i));
            canStart--;
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

// TODO: check empty password
void DownloadManager::startDownload(const DownloadInformation downloadInformation) {
    connect(downloadInformation.downloader, SIGNAL(progress(qint64,qint64)), this, SLOT(handleDownloadProgress(qint64,qint64)));
    connect(downloadInformation.downloader, SIGNAL(speed(float)), this, SLOT(handleDownloadSpeed(float)));
    connect(downloadInformation.downloader, SIGNAL(error(QString)), this, SLOT(handleDownloadError(QString)));
    connect(downloadInformation.downloader, SIGNAL(finished()), this, SLOT(handleDownloadFinished()));
    connect(downloadInformation.downloader, SIGNAL(receivedFilename(QString)), this, SLOT(handleDownloadFilename(QString)));
    connect(downloadInformation.downloader, SIGNAL(receivedSize(qint64)), this, SLOT(handleDownloadSize(qint64)));
    downloadInformation.downloader->start(
        LinkGenerator::GenerateFWLink(loginData.username, loginData.password, downloadInformation.url),
        downloadInformation.path);
}

void DownloadManager::handleDownloadError(QString msg) {
    for(int i=0; i<downloadList.length(); i++) {
        if(downloadList[i].downloader == sender()) {
            downloadList[i].downloader->disconnect();
            downloadList[i].error = msg;
            downloadList[i].speed = 0;
            downloadList[i].status = StatError;
            break;
        }
    }
    checkDownloads();
}

void DownloadManager::parseErrorMessage(int i) {
    QFile file(downloadList[i].path + downloadList[i].file);
    if(file.open(QIODevice::ReadOnly)) {
        QString content = file.readAll();
        file.close();
        QRegExp regExp("<p id='error'>.*</p>");
        if(regExp.indexIn(content) != -1) {
            QString err = regExp.capturedTexts()[0].mid(14);
            err = err.left(err.indexOf("</p>"));
            QTextDocument text;
            text.setHtml(err);
            downloadList[i].error = text.toPlainText();
        }
    }
}

void DownloadManager::handleDownloadFinished() {
    for(int i=0; i<downloadList.length(); i++) {
        if(downloadList[i].downloader == sender()) {
            downloadList[i].downloader->disconnect();
            downloadList[i].speed = 0;
            downloadList[i].status = StatFinished;
            // free-way.me error
            if(downloadList[i].file.startsWith("unnamed") && downloadList[i].size < 1024) {
                parseErrorMessage(i);
                if(downloadList[i].error.contains("File offline"))
                    downloadList[i].status = StatFileOffline;
                else if(downloadList[i].error.contains("Ungültiger Login"))
                    downloadList[i].status = StatLoginError;
                // free-way.me made a typo here:
                // "Ungütiger Hoster" should be "Ungültiger Hoster"
                // in case they fix this FDP should check for both strings
                else if(downloadList[i].error.contains(QRegExp("Ungül?tiger Hoster")))
                    downloadList[i].status = StatInvalidUrl;
                else
                    downloadList[i].status = StatFWError;
            }
            break;
        }
    }
    checkDownloads();
}

void DownloadManager::handleDownloadProgress(qint64 curr, qint64 size) {
    for(int i=0; i<downloadList.length(); i++) {
        if(downloadList[i].downloader == sender()) {
            downloadList[i].progress = curr;
            if(downloadList[i].size != size)
                downloadList[i].size = size;
            break;
        }
    }
}

void DownloadManager::handleDownloadSpeed(float Bps) {
    for(int i=0; i<downloadList.length(); i++) {
        if(downloadList[i].downloader == sender()) {
            downloadList[i].speed = Bps;
            break;
        }
    }
}

void DownloadManager::handleDownloadFilename(QString filename) {
    for(int i=0; i<downloadList.length(); i++) {
        if(downloadList[i].downloader == sender()) {
            downloadList[i].file = filename;
            break;
        }
    }
}

void DownloadManager::handleDownloadSize(qint64 size) {
    for(int i=0; i<downloadList.length(); i++) {
        if(downloadList[i].downloader == sender()) {
            downloadList[i].size = size;
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

DownloadManager::~DownloadManager() {
    saveDownloads();
    for(int i=0; i<downloadList.length(); i++) {
        downloadList[i].downloader->stop(false);
        // delete downloadList[i].downloader; // causes crashes on windows
    }
}

QJsonObject DownloadManager::download2Json(int idx) {
    QJsonObject returnValue;
    returnValue.insert("error", QJsonValue(downloadList.at(idx).error));
    returnValue.insert("file", QJsonValue(downloadList.at(idx).file));
    returnValue.insert("path", QJsonValue(downloadList.at(idx).path));
    returnValue.insert("progress", QJsonValue(downloadList.at(idx).progress));
    returnValue.insert("size", QJsonValue(downloadList.at(idx).size));
    returnValue.insert("status", QJsonValue(downloadList.at(idx).status));
    returnValue.insert("url", QJsonValue(downloadList.at(idx).url));
    return returnValue;
}

QString DownloadManager::getDownloadsFile() {
    QString path = QApplication::applicationDirPath();
    if(!path.endsWith("/"))
        path += "/";
    return tr("%1%2").arg(path).arg("downloads.json");
}

void DownloadManager::saveDownloads() {
    // trying to open file with downloads for writing
    QFile savedJsonDownloads(getDownloadsFile());
    if(savedJsonDownloads.open(QIODevice::WriteOnly)) {
        // generate json array
        QJsonArray jsonDownloads;
        for(int i=0; i<downloadList.length(); i++)
            jsonDownloads.append(download2Json(i));
        QJsonDocument jsonDoc(jsonDownloads);
        savedJsonDownloads.write(jsonDoc.toJson());
        savedJsonDownloads.close();
    }
}

void DownloadManager::stopDownload(int i) {
    downloadList[i].downloader->stop(false);
    downloadList[i].status = StatAborted;
    checkDownloads();
}

void DownloadManager::resetDownloadData(int i) {
    downloadList[i].downloader->stop(false);
    downloadList[i].error = "";
    downloadList[i].progress = 0;
    downloadList[i].size = 0; // because this could be an error file
    downloadList[i].speed = 0;
    downloadList[i].status = StatPending;
    downloadList[i].timeoutCounter = 0;
    downloadList[i].timeoutProgress = 0;
}

void DownloadManager::restartDownload(int i) {
    resetDownloadData(i);
    checkDownloads();
}

void DownloadManager::deleteDownload(int i) {
    downloadList[i].downloader->stop(false);
    delete downloadList[i].downloader;
    downloadList.removeAt(i);
}

void DownloadManager::addDownloadFromJson(QJsonObject obj) {
    DownloadInformation dwl;
    dwl.error = obj.value("error").toString();
    dwl.file = obj.value("file").toString();
    dwl.path = obj.value("path").toString();
    dwl.progress = obj.value("progress").toVariant().toLongLong();
    dwl.size = obj.value("size").toVariant().toLongLong();
    dwl.status = static_cast<DownloadStatus>(obj.value("status").toInt());
    dwl.url = obj.value("url").toString();
    if(dwl.status == StatInProgress)
        dwl.status = StatAborted;
    dwl.speed = 0;
    dwl.timeoutCounter = 0;
    dwl.timeoutProgress = 0;
    dwl.downloader = new FWDownload();
    downloadList.append(dwl);
}

void DownloadManager::loadDownloads() {
    // trying to open file with downloads for reading
    QFile savedJsonDownloads(getDownloadsFile());
    if(savedJsonDownloads.open(QIODevice::ReadOnly)) {
        // parse json data
        QJsonDocument doc(QJsonDocument::fromJson(savedJsonDownloads.readAll()));
        savedJsonDownloads.close();
        QJsonArray jsonDownloads = doc.array();
        for(int i=0; i<jsonDownloads.size(); i++) {
            addDownloadFromJson(jsonDownloads.at(i).toObject());
        }
        checkDownloads();
    }
}

} // end of namespace net
} // end of namespace fdp
