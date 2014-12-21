#include <QApplication>

#include <gui/mainwindow.h>

#ifdef unix
    #include "signal/unixsignallistener.h"
    #include <signal.h>
#endif

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    fdp::gui::MainWindow w;
    w.show();

    #ifdef unix
        fdp::signal::UnixSignalListener *sl = fdp::signal::UnixSignalListener::GetStaticInstance();
        sl->CatchSignal(SIGINT);
        QObject::connect(sl, SIGNAL(signalCaught(int)), &a, SLOT(quit()));
    #endif

    return a.exec();
}
