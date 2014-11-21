#ifndef DOWNLOADTABLECOLUMNS_H
#define DOWNLOADTABLECOLUMNS_H

namespace fdp {
namespace model {

enum DownloadTableColumns {
    ColumnFile = 0,
    ColumnStatus = 1,
    ColumnSize = 2,
    ColumnProgress = 3,
    ColumnSpeed = 4,
    ColumnRemaining = 5,
    ColumnUrl = 6,
    ColumnPath = 7
};

} // end of namespace model
} // end of namespace fdp

#endif // DOWNLOADTABLECOLUMNS_H
