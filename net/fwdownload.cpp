#include "fwdownload.h"

#include <QNetworkReply>
#include <QFileInfo>

namespace fdp {
namespace net {

FWDownload::FWDownload() :
    QObject(),
    naManager(new QNetworkAccessManager()),
    download(NULL),
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
        // error while opening
        if(!openFile()) {
            stopDownload();
            return;
        // emit filename and size
        } else {
            emit receivedFilename(QFileInfo(output).fileName());
            emit receivedSize(headerSize());
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
    if(download != NULL) {
        stopDownload();
        if(output.isOpen())
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

QString FWDownload::headerFilename() {
    // get the content disposition header
    QString returnValue = download->rawHeader("Content-Disposition");
    if(returnValue.isEmpty()) return "";
    // get the filename part
    int idx = returnValue.indexOf(QRegExp("filename\\*?=", Qt::CaseInsensitive));
    if(idx == -1) return "";
    returnValue = returnValue.mid(returnValue.indexOf("=", idx) + 1).trimmed();
    // Content-Disposition: INLINE; FILENAME= "an example.html"
    if(returnValue.contains(QRegExp("\".*\"")))
        return returnValue.split('"')[1];
    // Content-Disposition: attachment; filename*= UTF-8''%e2%82%ac%20rates
    if(returnValue.contains("''"))
        return QUrl::fromPercentEncoding(returnValue.mid(returnValue.indexOf("''") + 2).toLatin1());
    // Content-Disposition: Attachment; filename=example.html
    return returnValue;
}

QString FWDownload::alternativeFilename() {
    int idx = 0;
    while(QFile(tr("%1unnamed-%2").arg(path).arg(idx)).exists())
        idx++;
    return tr("unnamed-%1").arg(idx);
}

qint64 FWDownload::headerSize() {
    QString sizeString = download->rawHeader("Content-Length");
    return sizeString.toLongLong();
}

bool FWDownload::openFile() {
    // get filename
    QString filename = headerFilename();
    if(filename.isEmpty())
        filename = alternativeFilename();
    // try to open it
    output.setFileName(tr("%1%2").arg(path).arg(filename));
    if (!output.open(QIODevice::WriteOnly)) {
        emit error(tr("couln't open \"%1%2\" for writing").arg(path).arg(filename));
        return false;
    } else return true;
}

} // end of namespace net
} // end of namespace fdp
