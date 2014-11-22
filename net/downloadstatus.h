#ifndef DOWNLOADSTATUS_H
#define DOWNLOADSTATUS_H

namespace fdp {
namespace net {

enum DownloadStatus {
    StatInProgress =  0x01,
    StatPending =     0x02,
    StatTimeout =     0x03,
    StatFWError =     0x04,
    StatError =       0x05,
    StatAborted =     0x06,
    StatFileOffline = 0x07,
    StatInvalidUrl =  0x08,
    StatLoginError =  0x09,
    StatFinished =    0x0a,
};

} // end of namespace net
} // end of namespace fdp

#endif // DOWNLOADSTATUS_H
