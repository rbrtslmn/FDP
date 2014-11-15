#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QList>
#include <QTimer>

#include "fwdownload.h"

namespace fdp {
namespace net {

enum InformationType {
    InfoNewDownload,
    InfoFilename,
    InfoProgress,
    InfoSize,
    InfoSpeed,
    InfoState
};

enum DownloadStatus {
    StatPending,
    StatInProgress,
    StatFinished,
    StatError,
    StatTimeout,
    StatFWError
};

struct DownloadInformation {
    QString file;
    QString path;
    QString url;
    QString fwUrl;
    QString error;
    qint64 size;
    qint64 progress;
    float speed;
    DownloadStatus status;
    FWDownload *downloader;
    qint64 timeoutProgress;
    int timeoutCounter;
};

class DownloadManager : public QObject {

    Q_OBJECT

public:
    explicit DownloadManager(int parallelDownloads, int reloadSettings, QObject *parent = 0);
    void addLink(QString url, QString fwUrl, QString path);
    int numberOfDownloads() const;
    int numberOfDownloads(DownloadStatus status) const;
    DownloadInformation downloadAt(int index) const;

signals:
    void newInformation(int downloadIdx, net::InformationType prop);

public slots:
    void setParallelDownloads(int parallelDownloads);
    void checkDownloads();
    void setReloadSettings(int reloadSettings);

protected slots:
    void handleDownloadProgress(qint64 curr, qint64 size);
    void handleDownloadSpeed(float Bps);
    void handleDownloadFinished();
    void handleDownloadError(QString msg);
    void handleDownloadFilename(QString filename);
    void check4Timeout();

protected:
    void startDownload(DownloadInformation downloadInformation);
    bool shouldReload(int i);

protected:
    QList<DownloadInformation> downloadList;
    int parallelDownloads;
    int reloadSettings;
    QTimer timeoutTimer;

};

} // end of namespace net
} // end of namespace fdp

#endif // DOWNLOADMANAGER_H
