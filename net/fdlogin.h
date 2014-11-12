#ifndef FDLOGIN_H
#define FDLOGIN_H

#include <QObject>
#include <QNetworkAccessManager>

class FDLogin : public QObject {

    Q_OBJECT

public:
    FDLogin();
    ~FDLogin();
    void login(QString user, QString pass);

protected slots:
    void handleReply(QNetworkReply *reply);
    void readReply();

protected:
    QString generatePassHash(QString user, QString pass);

protected: // objects
    QNetworkAccessManager *naManager;

};

#endif // FDLOGIN_H
