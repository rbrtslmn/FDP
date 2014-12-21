#include "unixsignallistener.h"

#include <QDebug>

namespace fdp {
namespace signal {

UnixSignalListener::UnixSignalListener(QObject *parent) :
    QObject(parent)
{ }

void UnixSignalListener::handleSignal(int sig, siginfo_t *siginfo, void *context) {
    (void)siginfo;
    (void)context;
    emit GetStaticInstance()->signalCaught(sig);
}

bool UnixSignalListener::CatchSignal(int signo) {
    // create action
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    // use sigaction
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = UnixSignalListener::handleSignal;
    // set handler
    return sigaction(signo, &action, NULL) == 0;
}

UnixSignalListener *UnixSignalListener::GetStaticInstance() {
    static UnixSignalListener *StaticInstance = new UnixSignalListener();
    return StaticInstance;
}

} // end of namespace signal
} // end of namespace fdp
