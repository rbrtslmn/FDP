#ifndef DOWNLOADSTATUS_H
#define DOWNLOADSTATUS_H

namespace fdp {
namespace net {

enum DownloadStatus {
    StatInProgress,
    StatAbout2Start,
    StatPending,
    StatTimeout,
    StatFWError,
    StatError,
    StatAborted,
    StatNoTraffic,
    StatFileOffline,
    StatInvalidUrl,
    StatLoginError,
    StatFinished
};

} // end of namespace net
} // end of namespace fdp

#endif // DOWNLOADSTATUS_H
