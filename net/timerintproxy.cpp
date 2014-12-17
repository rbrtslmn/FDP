#include "timerintproxy.h"

TimerIntProxy::TimerIntProxy(int value, QObject *receiver, const char *member) :
    value(value),
    receiver(receiver),
    member(member),
    timer(new QTimer())
{
    connect(timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    connect(this, SIGNAL(timeout(int)), receiver, member);
}

TimerIntProxy::~TimerIntProxy() {
    timer->deleteLater();
}

void TimerIntProxy::handleTimeout() {
    emit timeout(value);
    deleteLater();
}

void TimerIntProxy::start(int ms) {
    timer->start(ms);
}

void TimerIntProxy::SingleShot(int ms, int value, QObject *receiver, const char *member) {
    TimerIntProxy *timer = new TimerIntProxy(value, receiver, member);
    timer->start(ms);
}
