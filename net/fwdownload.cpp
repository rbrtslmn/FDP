#include "fwdownload.h"

#include <QNetworkReply>
#include <QFileInfo>
#include <QTextDocument>

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
    // reset download info
    lastSpeedCheck = QDateTime::currentDateTime();
    lastProgress = 0;
    inProgress = false;
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
    // first data -> check for filename
    if(!inProgress) {
        filename = headerFilename();
        // read valid filename
        if(!filename.isEmpty()) {
            // file info
            emit receivedFilename(filename);
            emit receivedSize(headerSize());
            // file exists?
            file.setFileName(tr("%1%2").arg(path).arg(filename));
            if(file.exists()) {
                stopDownload();
                emit error(tr("File \"%1%2\" exists").arg(path).arg(filename));
                return;
            }
            // couldn't open file
            if(!file.open(QIODevice::WriteOnly)) {
                stopDownload();
                emit error(tr("couln't open \"%1%2\" for writing").arg(path).arg(filename));
                return;
            }
        // no filename found in header
        } else {
            // not a status message
            if(headerSize() > 1024) {
                stopDownload();
                emit error("No filename found in content disposition header");
                return;
            } else buffer = "";
        }
        inProgress = true;
    }
    // write data to file
    if(!filename.isEmpty()) {
        file.write(download->readAll());
    // buffer data
    } else {
        buffer.append(download->readAll());
    }
}

QString FWDownload::parseStatus() {
    QRegExp regExp("<p id='error'>.*</p>");
    if(regExp.indexIn(buffer) != -1) {
        QString err = regExp.capturedTexts()[0].mid(14);
        err = err.left(err.indexOf("</p>"));
        QTextDocument text;
        text.setHtml(err);
        return text.toPlainText();
    } else {
        return "Couldn't parse Free-Way.me status";
    }
}

void FWDownload::handleFinish() {
    // close file
    if(file.isOpen())
        file.close();
    // check redirect
    int statusCode = download->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode == 302) {
        QUrl url = download->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        download->disconnect();
        start(url.toString(), path);
    // buffered data
    } else if(filename.isEmpty()) {
        emit receivedStat(parseStatus());
    // file finished
    } else {
        emit finished();
    }
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

void FWDownload::stop() {
    if(download != NULL)
        stopDownload();
    if(!filename.isEmpty()) {
        if(file.isOpen())
            file.close();
        file.remove();
    }
}

void FWDownload::stopDownload() {
    download->abort();
    download->disconnect();
}

void FWDownload::handleError() {
    emit error(download->errorString());
    stop();
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
    // check if there is a following disposition-parm
    if(returnValue.contains(";"))
        returnValue = returnValue.mid(0, returnValue.indexOf(";")).trimmed();
    // Content-Disposition: attachment; filename*= UTF-8''%e2%82%ac%20rates
    if(returnValue.contains("''"))
        return QUrl::fromPercentEncoding(returnValue.mid(returnValue.indexOf("''") + 2).toLatin1());
    // Content-Disposition: Attachment; filename=example.html
    return returnValue;
}

qint64 FWDownload::headerSize() {
    QString sizeString = download->rawHeader("Content-Length");
    return sizeString.toLongLong();
}

} // end of namespace net
} // end of namespace fdp
