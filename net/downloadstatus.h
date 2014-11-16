#ifndef DOWNLOADSTATUS_H
#define DOWNLOADSTATUS_H

namespace fdp {
namespace net {

enum DownloadStatus {
    StatPending = 0,
    StatInProgress = 1,
    StatAborted = 2,
    StatFinished = 3,

    StatLoginError = -1,
    StatTimeout = -2,
    StatFWError = -3,
    StatError = -4,
    StatFileOffline = -5,
};

} // end of namespace net
} // end of namespace fdp

#endif // DOWNLOADSTATUS_H
