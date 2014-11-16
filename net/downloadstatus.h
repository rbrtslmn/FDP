#ifndef DOWNLOADSTATUS_H
#define DOWNLOADSTATUS_H

namespace fdp {
namespace net {

enum DownloadStatus {
    StatPending,
    StatInProgress,
    StatFinished,
    StatTimeout,
    StatError,
    StatFWError,
    StatFileOffline,
    StatLoginError
};

} // end of namespace net
} // end of namespace fdp

#endif // DOWNLOADSTATUS_H
