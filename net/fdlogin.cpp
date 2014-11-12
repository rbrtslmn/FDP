#include "fdlogin.h"

#include <QCryptographicHash>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

FDLogin::FDLogin() :
    QObject(),
    naManager(new QNetworkAccessManager(this))
{
    connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleReply(QNetworkReply*)));
}

FDLogin::~FDLogin() {
    delete naManager;
}

QString FDLogin::generatePassHash(QString user, QString pass) {
    return QCryptographicHash::hash(user.toLower().append(pass).toLatin1(), QCryptographicHash::Sha1);
}

void FDLogin::handleReply(QNetworkReply *reply) {
    connect(reply, SIGNAL(readyRead()), this, SLOT(readReply()));
}

void FDLogin::readReply() {
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if(reply) {
        QString answer = "";
        while(reply->bytesAvailable()) {
            answer += reply->readAll();
        }
        qDebug() << answer;
    }
}

void FDLogin::login(QString user, QString pass) {
    QString url = "https://www.free-way.me/ajax/jd.php?";
    url += "user="  + user.toLatin1().toPercentEncoding();
    url += "&pass=" + generatePassHash(user, pass).toLatin1().toPercentEncoding();
    url += "&id=1";
    naManager->get(QNetworkRequest(QUrl(url)));
}
