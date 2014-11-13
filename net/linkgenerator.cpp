#include "linkgenerator.h"

#include <QCryptographicHash>

#include <QDebug>

namespace fdp {
namespace net {

QString LinkGenerator::GenerateFWLink(QString username, QString password, QString url) {
    QString returnValue = "https://www.free-way.me/load.php?url=";
    returnValue += url.toLatin1().toPercentEncoding();
    returnValue += "&user=";
    returnValue += username.toLatin1().toPercentEncoding();
    returnValue += "&pw=";
    returnValue += GeneratePasswordHash(username, password);
    returnValue += "&json";
    return returnValue;
}

QString LinkGenerator::GeneratePasswordHash(QString username, QString password) {
    return QCryptographicHash::hash(username.toLower().append(password).toLatin1(), QCryptographicHash::Sha1).toHex();
}

} // end of namespace net
} // end of namespace fdp
