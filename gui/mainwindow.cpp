#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <net/fwdownload.h>

namespace fdp {
namespace gui {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    downloadTable(new model::DownloadTable()),
    daemon(new net::HTTPDaemon()),
    debugDl(new net::FWDownload())
{
    ui->setupUi(this);
    setWindowTitle("Free-Way.me Download Program");

    ui->tableView->setModel(downloadTable);

    // *************** start of debugging
    daemon->listen();

    connect(daemon, SIGNAL(receivedLinks(QString)), ui->plainTextEdit, SLOT(setPlainText(QString)));

    connect(debugDl, SIGNAL(speed(float)), this, SLOT(debug1(float)));


    // debugDl->start("http://cdimage.debian.org/debian-cd/7.7.0/multi-arch/iso-cd/debian-7.7.0-amd64-i386-netinst.iso", "/home/r/test.iso");
    // debugDl->start("http://caesar.acc.umu.se/debian-cd/7.7.0/multi-arch/iso-cd/debian-7.7.0-amd64-i386-netinst.iso", "/home/r/test.iso");


    // debugDl->start("http://download.qt-project.org/official_releases/qt/5.3/5.3.2/qt-opensource-linux-x64-android-5.3.2.run", "/home/r/test.iso");
}

void MainWindow::debug1(float Bps) {
    int i=0;
    while(Bps >= 1000 && i<2) {
        Bps /= 1024;
        i++;
    }
    ui->statusBar->showMessage(tr("Speed: %1 %2%3").arg(Bps).arg(i<1?"":(i<2?"ki":"Mi")).arg("B/s"));
}

MainWindow::~MainWindow() {
    daemon->stop();
    delete daemon;
    delete downloadTable;
    delete ui;
}

} // end of namespace gui
} // end of namespace fdp
