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

void FWDownload::start(const QString url, const QString file) {
    // open file
    output.setFileName(file);
    if (!output.open(QIODevice::WriteOnly)) {
        emit error(tr("couln't open \"%1\" for writing").arg(file));
        return;
    }
    // reset download speed info
    lastSpeedCheck = QDateTime::currentDateTime();
    lastProgress = 0;
    request = QNetworkRequest(url);
    download = naManager->get(request);
    connect(download, SIGNAL(downloadProgress(qint64,qint64)),    this, SLOT(handleProgress(qint64,qint64)));
    connect(download, SIGNAL(readyRead()),                        this, SLOT(handleData()));
    connect(download, SIGNAL(finished()),                         this, SLOT(handleFinish()));
    connect(download, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleError()));
    connect(download, SIGNAL(sslErrors(QList<QSslError>)),        this, SLOT(handleError()));
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

void FWDownload::handleError() {
    emit error(download->errorString());
}

void FWDownload::handleData() {
    while(download->bytesAvailable()) {
        output.write(download->readAll());
    }
}

void FWDownload::stop() {

    // TODO: test auf running download (wrong ptr + removing complete file possible)

    download->abort();
    download->disconnect();
    output.close();
    output.remove();
}

void FWDownload::handleFinish() {
    output.close();
    // check redirect
    int statusCode = download->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 302) {
        QUrl url = download->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        download->disconnect();
        start(url.toString(), output.fileName());
    } else emit finished();
}

} // end of namespace net
} // end of namespace fdp
