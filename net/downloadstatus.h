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
    StatNoTraffic =   0x07,
    StatFileOffline = 0x08,
    StatInvalidUrl =  0x09,
    StatLoginError =  0x0a,
    StatFinished =    0x0b,
};

} // end of namespace net
} // end of namespace fdp

#endif // DOWNLOADSTATUS_H
