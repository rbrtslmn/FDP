#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QList>
#include <QTimer>
#include <QModelIndexList>

#include "fwdownload.h"

#include "downloadstatus.h"
#include "downloadinformation.h"
#include "logindata.h"

namespace fdp {
namespace net {

class DownloadManager : public QObject {

    Q_OBJECT

public:
    explicit DownloadManager(const int parallelDownloads, const int reloadSettings, const LoginData loginData, QObject *parent = 0);
    ~DownloadManager();
    void addLink(const QString url, const QString path);
    int numberOfDownloads() const;
    int numberOfDownloads(const DownloadStatus status) const;
    DownloadInformation downloadAt(const int index) const;

    void stopDownload(int i);
    void restartDownload(int i);
    void deleteDownload(int i);

public slots:
    void setParallelDownloads(int parallelDownloads);
    void checkDownloads();
    void setReloadSettings(int reloadSettings);
    void setLoginData(QString username, QString password);

protected slots:
    void handleDownloadProgress(qint64 curr, qint64 size);
    void handleDownloadSpeed(float Bps);
    void handleDownloadFinished();
    void handleDownloadError(QString msg);
    void handleDownloadFilename(QString filename);
    void handleDownloadSize(qint64 size);
    void handleDownloadStat(QString status);
    void check4Timeout();

protected:
    void startDownload(int i);
    bool shouldReload(const int i);
    void saveDownloads();
    void loadDownloads();
    QString getDownloadsFile();
    void addDownloadFromJson(QJsonObject obj);
    QJsonObject download2Json(int idx);
    void resetDownloadData(int i);

protected:
    QList<DownloadInformation> downloadList;
    int parallelDownloads;
    int reloadSettings;
    QTimer timeoutTimer;
    LoginData loginData;

};

} // end of namespace net
} // end of namespace fdp

#endif // DOWNLOADMANAGER_H
