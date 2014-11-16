#ifndef LOGINDATA_H
#define LOGINDATA_H

#include <QString>

namespace fdp {
namespace net {

struct LoginData {
    QString username;
    QString password;
};

} // end of namespace net
} // end of namespace fdp

#endif // LOGINDATA_H
