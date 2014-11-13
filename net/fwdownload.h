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
    void stop(bool deleteFile);

signals:
    void speed(float Bps);
    void progress(qint64 curr, qint64 size);
    void finished();
    void error(QString message);
    void receivedFilename(QString filename);

protected slots:
    void handleProgress(qint64 curr, qint64 size);
    void handleData();
    void handleFinish();
    void handleError();

protected:
    void stopDownload();
    bool openFile();

protected:
    QString path;
    QString url;
    QNetworkAccessManager *naManager;
    QNetworkReply *download;
    QNetworkRequest request;
    QDateTime lastSpeedCheck;
    qint64 lastProgress;
    int speedIntervalMs;
    QFile output;

};

} // end of namespace net
} // end of namespace fdp

#endif // FWDOWNLOAD_H
