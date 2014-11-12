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

    downloadManager = new model::DownloadManager(ui->spinBox->value());
    ui->tableView->setModel(downloadTable);

    daemon->listen();

    connect(daemon, SIGNAL(receivedLinks(QString)), ui->plainTextEdit, SLOT(setPlainText(QString)));
    connect(ui->spinBox, SIGNAL(valueChanged(int)), downloadManager, SLOT(setParallelDownloads(int)));
    connect(ui->toolButton, SIGNAL(clicked()), this, SLOT(choosePath()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(startDownloads()));
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
        downloadManager->addLink(ui->lineEdit->text(), ui->lineEdit_2->text(), ui->lineEdit_3->text(), list.at(i));
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
    // user interface
    settings.setValue("username", ui->lineEdit->text());
    settings.setValue("password", ui->checkBox->checkState()?ui->lineEdit_2->text():"");
    settings.setValue("path",     ui->lineEdit_3->text());
    settings.setValue("parallel", ui->spinBox->value());
    settings.setValue("savepass", ui->checkBox->checkState());
    // size
    settings.setValue("width", width());
    settings.setValue("height", height());
}

void MainWindow::loadSettings() {
    QSettings settings("Codingspezis", "FDP");
    // user interface
    ui->lineEdit->setText(settings.value("username").toString());
    ui->lineEdit_2->setText(settings.value("password").toString());
    ui->lineEdit_3->setText(settings.value("path").toString());
    ui->spinBox->setValue(settings.value("parallel").toInt());
    ui->checkBox->setChecked(settings.value("savepass").toBool());
    // size
    resize(settings.value("width").toInt(), settings.value("height").toInt());
}

} // end of namespace gui
} // end of namespace fdp
