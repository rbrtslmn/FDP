#include "httpdaemon.h"

#include <QTcpSocket>
#include <QUrl>
#include <QScriptEngine>

#include <crypt/aesdecoder.h>

namespace fdp {
namespace net {

HTTPDaemon::HTTPDaemon() :
    QObject(),
    tcpServer(new QTcpServer()),
    port(9666)
{
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(handleConnect()));
}

HTTPDaemon::~HTTPDaemon() {
    stop();
    delete tcpServer;
}

bool HTTPDaemon::listen() {
    if(!tcpServer->isListening())
        return tcpServer->listen(QHostAddress::Any, port);
    else return false;
}

void HTTPDaemon::stop() {
    if(tcpServer->isListening())
        tcpServer->close();
}

void HTTPDaemon::handleConnect() {
    while(tcpServer->hasPendingConnections()) {
        QTcpSocket *client = tcpServer->nextPendingConnection();
        connect(client, SIGNAL(readyRead()), this, SLOT(handleConnectionData()));
        connect(client, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));
    }
}

QString HTTPDaemon::generateHTTPResponse(const QString text) {
    QString returnValue = QString(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: %1\r\n\r\n%2").arg(text.length()).arg(text);
    return returnValue;
}

qint64 HTTPDaemon::parseLength(QString data) {
    int idx = data.indexOf("Content-Length:");
    if(idx != -1) {
        QRegExp lengthRegExp("[0-9]+");
        if(lengthRegExp.indexIn(data, idx) != -1)
            return lengthRegExp.capturedTexts().at(0).toLongLong();
    }
    return -1;
}

int HTTPDaemon::knownClient(QTcpSocket *client) {
    for(int i=0; i<requests.size(); i++) {
        if(client == requests[i].client)
            return i;
    }
    return -1;
}

void HTTPDaemon::handleConnectionData() {
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    if(client && client->bytesAvailable()) {
        int idx = knownClient(client);
        // first data from client
        if(idx == -1) {
            HTTPRequest request;
            request.client = client;
            request.data = client->readAll();
            request.length = parseLength(request.data);
            requests.append(request);
            idx = requests.length() - 1;
        // second or later received data
        } else {
            requests[idx].data += client->readAll();
        }
        int headerEnd = requests[idx].data.indexOf("\r\n\r\n");
        if(headerEnd == -1 // invalid header block
        || requests[idx].length == -1 // no content length header
        || requests[idx].data.mid(headerEnd + 4).length() >= requests[idx].length) { // received all wanted bytes
            // handle data
            handleRequest(requests[idx].data, client);
            client->flush();
            client->close();
            client->deleteLater();
            requests.removeAt(idx);
        }
    }
}

void HTTPDaemon::handleRequest(const QString request, QTcpSocket *client) {
    // running request (flash)
    if(request.contains(QRegExp("GET /?flash.* ")))
        client->write(generateHTTPResponse("JDownloader").toLatin1());
    // running request (js)
    else if(request.contains(QRegExp("GET /?jdcheck.js ")))
        client->write(generateHTTPResponse("jdownloader=true;").toLatin1());
    // running request (crossdomain.xml)
    else if(request.contains(QRegExp("GET /?crossdomain.xml "))) {
        QString cdf = "<?xml version=\"1.0\"?> <cross-domain-policy> <allow-access-from domain=\"*\" /> </cross-domain-policy>";
        client->write(generateHTTPResponse(cdf).toLatin1());
    }
    // link post (encrypted)
    else if(request.contains(QRegExp("POST /?flash/addcrypted2/? "))) {
        if(parseEncryptedLinks(request))
            client->write(generateHTTPResponse("success").toLatin1());
        else
            client->write(generateHTTPResponse("fail").toLatin1());
    }
    // link post (plain)
    else if(request.contains(QRegExp("POST /?flash/add/? "))) {
        if(parsePlainLinks(request))
            client->write(generateHTTPResponse("success").toLatin1());
        else
            client->write(generateHTTPResponse("fail").toLatin1());
    }
}

bool HTTPDaemon::parseEncryptedLinks(const QString data) {
    QString jk;
    QString crypted;
    if(parsePostValue(data, "jk", jk) &&
    parsePostValue(data, "crypted", crypted)) {
        QScriptEngine scriptEngine;
        QScriptValue jse = scriptEngine.evaluate(jk + ";f();");
        QByteArray cipher = QByteArray::fromBase64(crypted.toLatin1());
        QByteArray key =    QByteArray::fromHex(jse.toString().toLatin1());
        QByteArray ba;
        bool returnValue = crypt::AESDecoder::DecodeCNL(cipher, key, ba);
        if(returnValue)
            emit receivedLinks(QString(ba));
        return returnValue;
    }
    return false;
}

bool HTTPDaemon::parsePostValue(const QString data, const QString name, QString &value) {
    int idxPostStart = data.indexOf("\r\n\r\n");
    if(idxPostStart == -1)
        return false;
    QString postData = data.mid(idxPostStart).trimmed();
    QStringList posts = postData.split("&");
    for(int i=0; i<posts.length(); i++) {
        if(posts.at(i).startsWith(name) && (posts.at(i).at(name.length()) == '=')) {
            QString tmp = posts.at(i).mid(name.length() + 1);
            tmp = tmp.replace("+", " ");
            value = QUrl::fromPercentEncoding(tmp.toLatin1());
            return true;
        }
    }
    return false;
}

bool HTTPDaemon::parsePlainLinks(const QString data) {
    QString urls;
    if(parsePostValue(data, "urls", urls)) {
        emit receivedLinks(urls);
        return true;
    }
    return false;
}

void HTTPDaemon::handleDisconnect() {
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    if(client)
        client->deleteLater();
}

} // end of namespace net
} // end of namespace fdp
