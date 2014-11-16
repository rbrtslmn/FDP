#ifndef INFORMATIONTYPE_H
#define INFORMATIONTYPE_H

namespace fdp {
namespace net {

enum InformationType {
    InfoNewDownload,
    InfoDownloadDeleted,
    InfoFilename,
    InfoProgress,
    InfoSize,
    InfoSpeed,
    InfoState
};

} // end of namespace net
} // end of namespace fdp

#endif // INFORMATIONTYPE_H
