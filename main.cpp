#include <QApplication>

#include <gui/mainwindow.h>

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    fdp::gui::MainWindow w;
    w.show();

    return a.exec();
}
