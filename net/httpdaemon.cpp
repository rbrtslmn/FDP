#include "httpdaemon.h"

#include <QTcpSocket>
#include <QUrl>
#include <QScriptEngine>

#include "aesdecoder.h"

HTTPDaemon::HTTPDaemon() :
    QObject(),
    tcpServer(new QTcpServer()),
    port(9666)
{
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));
}

bool HTTPDaemon::listen() {
    return tcpServer->listen(QHostAddress::Any, port);
}

void HTTPDaemon::stop() {
    tcpServer->close();
}

void HTTPDaemon::handleNewConnection() {
    while(tcpServer->hasPendingConnections()) {
        QTcpSocket *client = tcpServer->nextPendingConnection();
        connect(client, SIGNAL(readyRead()), this, SLOT(handleConnectionData()));
        connect(client, SIGNAL(destroyed()), this, SLOT(handleConnectionDestoyed()));
    }
}

QString HTTPDaemon::generateHTTPResponse(const QString text) {
    QString returnValue = QString(
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: %1\r\n\r\n%2").arg(text.length()).arg(text);
    return returnValue;
}

void HTTPDaemon::handleConnectionData() {
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    if(client) {
        QString request = "";
        while(client->bytesAvailable()) {
            request += client->readAll();
        }
        handleRequest(request, client);
        client->close();
    }
}

void HTTPDaemon::handleRequest(const QString request, QTcpSocket *client) {
    // running request (flash)
    if(request.contains(QRegExp("GET /?flash/? ")))
        client->write(generateHTTPResponse("JDownloader").toLatin1());
    // running request (js)
    if(request.contains(QRegExp("GET /?jdcheck.js ")))
        client->write(generateHTTPResponse("jdownloader=true;").toLatin1());
    // link post (encrypted)
    if(request.contains(QRegExp("POST /?flash/addcrypted2/? "))) {
        if(parseEncryptedLinks(request))
            client->write(generateHTTPResponse("success").toLatin1());
        else
            client->write(generateHTTPResponse("fail").toLatin1());
    }
    // link post (plain)
    if(request.contains(QRegExp("POST /?flash/add/? "))) {
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
        bool returnValue = AESDecoder::Decode(cipher, key, ba);
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

void HTTPDaemon::handleConnectionDestoyed() {
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    if(client) {
        client->disconnect();
        qDebug() << client->localAddress().toString() << " disconnected";
    }
}
