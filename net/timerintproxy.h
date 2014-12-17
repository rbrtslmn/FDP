#ifndef TIMERINTPROXY_H
#define TIMERINTPROXY_H

#include <QObject>
#include <QTimer>

class TimerIntProxy : public QObject {

    Q_OBJECT

public:
    static void SingleShot(int ms, int value, QObject *receiver, const char *member);

signals:
    void timeout(int value);

protected slots:
    void handleTimeout();

protected:
    TimerIntProxy(int value, QObject *receiver, const char *member);
    ~TimerIntProxy();
    void start(int ms);

protected:
    int value;
    QObject *receiver;
    const char *member;
    QTimer *timer;

};

#endif // TIMERINTPROXY_H
