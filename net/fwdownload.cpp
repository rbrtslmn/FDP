#include "fwdownload.h"

#include <QNetworkReply>

namespace fdp {
namespace net {

FWDownload::FWDownload() :
    QObject(),
    naManager(new QNetworkAccessManager()),
    speedIntervalMs(1000)
{ }

FWDownload::~FWDownload() {
    delete naManager;
}

void FWDownload::start(const QString url, const QString path) {
    // save given data
    this->url  = url;
    this->path = path;
    if(!this->path.endsWith("/"))
        this->path += "/";
    // reset download speed info
    lastSpeedCheck = QDateTime::currentDateTime();
    lastProgress = 0;
    // send request
    request = QNetworkRequest(url);
    download = naManager->get(request);
    // setup connections
    connect(download, SIGNAL(downloadProgress(qint64,qint64)),    this, SLOT(handleProgress(qint64,qint64)));
    connect(download, SIGNAL(readyRead()),                        this, SLOT(handleData()));
    connect(download, SIGNAL(finished()),                         this, SLOT(handleFinish()));
    connect(download, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleError()));
    connect(download, SIGNAL(sslErrors(QList<QSslError>)),        this, SLOT(handleError()));
}

void FWDownload::handleData() {
    // open file
    if(!output.isOpen()) {
        if(!openFile()) {
            stopDownload();
            return;
        }
    }
    // write to file
    while(download->bytesAvailable()) {
        output.write(download->readAll());
    }
}

void FWDownload::handleFinish() {
    if(output.isOpen())
        output.close();
    // check redirect
    int statusCode = download->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 302) {
        QUrl url = download->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        download->disconnect();
        start(url.toString(), path);
    } else emit finished();
}

void FWDownload::handleProgress(qint64 curr, qint64 size) {
    // calculate speed
    QDateTime now = QDateTime::currentDateTime();
    int elapsedMs = lastSpeedCheck.msecsTo(now);
    if(elapsedMs >= speedIntervalMs) { // update speed
        emit speed(1000 * (curr - lastProgress) / (float)elapsedMs);
        lastSpeedCheck = now;
        lastProgress = curr;
    }
    // progress
    emit progress(curr, size);
}

void FWDownload::stop(bool deleteFile) {
    // if download is running
    if(output.isOpen()) {
        stopDownload();
        output.close();
        if(deleteFile)
            output.remove();
    }
}

void FWDownload::stopDownload() {
    download->abort();
    download->disconnect();
}

void FWDownload::handleError() {
    emit error(download->errorString());
    stop(false);
}

bool FWDownload::openFile() {
    // parse filename
    QString remoteFile = download->rawHeader("Content-Disposition");
    int idx = remoteFile.indexOf(QRegExp("filename=\".*\""));
    if(idx != -1) {
        remoteFile = remoteFile.mid(idx).split("\"")[1];
    } else {
        int idx = 0;
        while(QFile(tr("%1unnamed-%2").arg(path).arg(idx)).exists())
            idx++;
        remoteFile = tr("unnamed-%1").arg(idx);
    }
    emit receivedFilename(remoteFile);
    // try to open it
    output.setFileName(tr("%1%2").arg(path).arg(remoteFile));
    if (!output.open(QIODevice::WriteOnly)) {
        emit error(tr("couln't open \"%1%2\" for writing").arg(path).arg(remoteFile));
        return false;
    } else return true;
}

} // end of namespace net
} // end of namespace fdp
