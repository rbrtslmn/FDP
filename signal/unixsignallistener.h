#ifndef UNIXSIGNALLISTENER_H
#define UNIXSIGNALLISTENER_H

#include <QObject>
#include <signal.h>

namespace fdp {
namespace signal {

class UnixSignalListener : public QObject {

    Q_OBJECT

public:
    static UnixSignalListener* GetStaticInstance();
    static bool CatchSignal(int signo);

signals:
    void signalCaught(int signo);

protected:
    UnixSignalListener(QObject *parent = 0);
    static void handleSignal(int sig, siginfo_t *siginfo, void *context);

};

} // end of namespace signal
} // end of namespace fdp

#endif // UNIXSIGNALLISTENER_H
