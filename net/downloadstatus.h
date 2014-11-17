#ifndef DOWNLOADSTATUS_H
#define DOWNLOADSTATUS_H

namespace fdp {
namespace net {

enum DownloadStatus {
    // normal states
    StatPending = 0,
    StatInProgress = 1,
    StatAborted = 2,
    StatFinished = 3,
    // errors
    StatLoginError = -1,
    StatTimeout = -2,
    StatFWError = -3,
    StatError = -4,
    StatInvalidUrl = -5,
    StatFileOffline = -6
};

} // end of namespace net
} // end of namespace fdp

#endif // DOWNLOADSTATUS_H
