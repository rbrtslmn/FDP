#ifndef HTTPDAEMON_H
#define HTTPDAEMON_H

#include <QTcpServer>

namespace fdp {
namespace net {

class HTTPDaemon : public QObject {

    Q_OBJECT

public:
    HTTPDaemon();

public:
    bool listen();
    void stop();

    QString generateHTTPResponse(const QString text);

signals:
    void receivedLinks(QString links);

protected slots:
    void handleNewConnection();
    void handleConnectionDestoyed();
    void handleConnectionData();

protected: // methods
    bool parseEncryptedLinks(const QString data);
    bool parsePlainLinks(const QString data);
    bool parsePostValue(const QString data, const QString name, QString &value);
    void handleRequest(const QString request, QTcpSocket *client);

protected: // objects
    QTcpServer *tcpServer;
    quint16 port;

};

} // end of namespace net
} // end of namespace fdp

#endif // HTTPDAEMON_H
