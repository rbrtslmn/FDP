#include <QFileDialog>
#include <QSettings>

#include <net/fwdownload.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    loadSettings();

    ui->tableView->setModel(downloadTable);



    connect(ui->toolButton, SIGNAL(clicked()), this, SLOT(choosePath()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(startDownloads()));


    // *************** start of debugging
    daemon->listen();

    connect(daemon, SIGNAL(receivedLinks(QString)), ui->plainTextEdit, SLOT(setPlainText(QString)));

    connect(debugDl, SIGNAL(speed(float)), this, SLOT(debug1(float)));
    connect(debugDl, SIGNAL(error(QString)), ui->statusBar, SLOT(showMessage(QString)));

    // debugDl->start("http://cdimage.debian.org/debian-cd/7.7.0/multi-arch/iso-cd/debian-7.7.0-amd64-i386-netinst.iso", "/home/r/test.iso");
    // debugDl->start("http://caesar.acc.umu.se/debian-cd/7.7.0/multi-arch/iso-cd/debian-7.7.0-amd64-i386-netinst.iso", "/home/r/test.iso");

#if 1
#endif
}

MainWindow::~MainWindow() {
    saveSettings();
    daemon->stop();
    delete daemon;
    delete downloadTable;
    delete ui;
}

void MainWindow::choosePath() {
    QFileDialog dirDialog;
    dirDialog.setFileMode(QFileDialog::DirectoryOnly);
    dirDialog.setDirectory(ui->lineEdit_3->text());
    if(dirDialog.exec()) {
        if(!dirDialog.selectedFiles().isEmpty())
            ui->lineEdit_3->setText(dirDialog.selectedFiles().at(0));
    }
}

void MainWindow::startDownloads() {
    QStringList list = ui->plainTextEdit->toPlainText().split("\n");
    for(int i=0; i<list.length(); i++) {

    }
}

void MainWindow::debug1(float Bps) {
    int i=0;
    while(Bps >= 1000 && i<2) {
        Bps /= 1024;
        i++;
    }
    ui->statusBar->showMessage(tr("Speed: %1 %2%3").arg(Bps).arg(i<1?"":(i<2?"ki":"Mi")).arg("B/s"));
}

void MainWindow::saveSettings() {
    QSettings settings("Codingspezis", "FDP");
    settings.setValue("username", ui->lineEdit->text());
    settings.setValue("password", ui->lineEdit_2->text());
    settings.setValue("path",     ui->lineEdit_3->text());
}

void MainWindow::loadSettings() {
    QSettings settings("Codingspezis", "FDP");
    ui->lineEdit->setText(settings.value("username").toString());
    ui->lineEdit_2->setText(settings.value("password").toString());
    ui->lineEdit_3->setText(settings.value("path").toString());
}

} // end of namespace gui
} // end of namespace fdp
