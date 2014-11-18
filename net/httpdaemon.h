#ifndef HTTPDAEMON_H
#define HTTPDAEMON_H

#include <QTcpServer>

namespace fdp {
namespace net {

struct HTTPRequest {
    qint64 length;
    QString data;
    QTcpSocket *client;
};

class HTTPDaemon : public QObject {

    Q_OBJECT

public:
    HTTPDaemon();
    ~HTTPDaemon();

public:
    bool listen();
    void stop();

    QString generateHTTPResponse(const QString text);

signals:
    void receivedLinks(QString links);

protected slots:
    void handleConnect();
    void handleDisconnect();
    void handleConnectionData();

protected: // methods
    bool parseEncryptedLinks(const QString data);
    bool parsePlainLinks(const QString data);
    bool parsePostValue(const QString data, const QString name, QString &value);
    void handleRequest(const QString request, QTcpSocket *client);
    qint64 parseLength(QString data);
    int knownClient(QTcpSocket *client);

protected: // objects
    QTcpServer *tcpServer;
    quint16 port;
    QList<HTTPRequest> requests;

};

} // end of namespace net
} // end of namespace fdp

#endif // HTTPDAEMON_H
