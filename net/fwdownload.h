#ifndef FWDOWNLOAD_H
#define FWDOWNLOAD_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QFile>

class FWDownload : public QObject {

    Q_OBJECT

public:
    FWDownload();
    ~FWDownload();

    void start(const QString url, const QString file);
    void stop();

signals:
    void speed(float Bps);
    void progress(qint64 curr, qint64 size);
    void finished();
    void error(QString message);

protected slots:
    void handleProgress(qint64 curr, qint64 size);
    void handleData();
    void handleFinish();
    void handleError();

protected:
    QNetworkAccessManager *naManager;
    QNetworkReply *download;
    QNetworkRequest request;
    QDateTime lastSpeedCheck;
    qint64 lastProgress;
    int speedIntervalMs;
    QFile output;


};

#endif // FWDOWNLOAD_H
