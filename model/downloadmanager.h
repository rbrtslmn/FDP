#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QString>
#include <QList>

#include <net/fwdownload.h>

namespace fdp {
namespace model {

struct PendingDownload {
    QString url;
    QString path;
};

class DownloadManager : public QObject {

    Q_OBJECT

public:
    DownloadManager(int parallelDownloads);
    void addLink(QString username, QString password, QString path, QString url);

public slots:
    void setParallelDownloads(int parallelDownloads);

protected:
    QString generatePasswordHash(QString username, QString password);
    QString generatePremiumLink(QString username, QString password, QString url);
    void checkDownloads();

protected:
    QList<net::FWDownload *> activeDownloads;
    QList<PendingDownload> pendingDownloads;
    int parallelDownloads;

};

} // end of namespace model
} // end of namespace fdp

#endif // DOWNLOADMANAGER_H
