#ifndef LINKGENERATOR_H
#define LINKGENERATOR_H

#include <QObject>
#include <QString>

namespace fdp {
namespace net {

class LinkGenerator : public QObject {

    Q_OBJECT

public:
    static QString GenerateFWLink(QString username, QString password, QString url);
    static QString GeneratePasswordHash(QString username, QString password);

};

} // end of namespace net
} // end of namespace fdp

#endif // LINKGENERATOR_H
