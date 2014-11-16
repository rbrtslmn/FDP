#ifndef DOWNLOADINFORMATION_H
#define DOWNLOADINFORMATION_H

#include <QString>

#include "downloadstatus.h"
#include "fwdownload.h"

namespace fdp {
namespace net {

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

} // end of namespace net
} // end of namespace fdp

#endif // DOWNLOADINFORMATION_H
