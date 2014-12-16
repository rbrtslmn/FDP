#ifndef FWDOWNLOAD_H
#define FWDOWNLOAD_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QFile>

namespace fdp {
namespace net {

class FWDownload : public QObject {

    Q_OBJECT

public:
    FWDownload();
    ~FWDownload();
    void start(const QString url, const QString path);
    void stop();

signals:
    void speed(float Bps);
    void progress(qint64 curr, qint64 size);
    void finished();
    void error(QString message);
    void receivedFilename(QString filename);
    void receivedSize(qint64 size);
    void receivedStat(QString status);

protected slots:
    void handleProgress(qint64 curr, qint64 size);
    void handleData();
    void handleFinish();
    void handleError();

protected:
    void stopDownload();
    QString headerFilename();
    qint64 headerSize();
    QString parseStatus();

protected:
    // download info
    QString url;
    QString path;
    QString filename;
    // download file/buffer
    QFile file;
    QByteArray buffer;

    bool inProgress;
    QNetworkAccessManager *naManager;
    QNetworkReply *download;
    QNetworkRequest request;
    QDateTime lastSpeedCheck;
    qint64 lastProgress;
    int speedIntervalMs;

};

} // end of namespace net
} // end of namespace fdp

#endif // FWDOWNLOAD_H
