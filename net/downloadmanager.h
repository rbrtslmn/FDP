#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QList>
#include <QTimer>

#include "fwdownload.h"

#include "informationtype.h"
#include "downloadstatus.h"
#include "downloadinformation.h"

namespace fdp {
namespace net {

class DownloadManager : public QObject {

    Q_OBJECT

public:
    explicit DownloadManager(const int parallelDownloads, const int reloadSettings, QObject *parent = 0);
    void addLink(const QString url, const QString fwUrl, const QString path);
    int numberOfDownloads() const;
    int numberOfDownloads(const DownloadStatus status) const;
    DownloadInformation downloadAt(const int index) const;

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
    void startDownload(const DownloadInformation downloadInformation);
    bool shouldReload(const int i);

protected:
    QList<DownloadInformation> downloadList;
    int parallelDownloads;
    int reloadSettings;
    QTimer timeoutTimer;

};

} // end of namespace net
} // end of namespace fdp

#endif // DOWNLOADMANAGER_H
