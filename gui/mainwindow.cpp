#include <QFileDialog>
#include <QSettings>

#include <net/fwdownload.h>
#include <net/linkgenerator.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace fdp {
namespace gui {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    daemon(new net::HTTPDaemon())
{
    ui->setupUi(this);
    setWindowTitle("Free-Way.me Download Program");
    loadSettings();

    downloadManager = new net::DownloadManager(ui->spinBox->value());
    downloadTable = new model::DownloadTable(downloadManager);
    ui->tableView->setModel(downloadTable);

    daemon->listen();

    connect(daemon, SIGNAL(receivedLinks(QString)), ui->plainTextEdit, SLOT(appendPlainText(QString)));
    connect(ui->spinBox, SIGNAL(valueChanged(int)), downloadManager, SLOT(setParallelDownloads(int)));
    connect(ui->toolButton, SIGNAL(clicked()), this, SLOT(choosePath()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(addDownloads()));
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

void MainWindow::addDownloads() {
    QStringList list = ui->plainTextEdit->toPlainText().split("\n");
    for(int i=0; i<list.length(); i++) {
        QString url = list.at(i).trimmed();
        if(!url.isEmpty())
            downloadManager->addLink(url, net::LinkGenerator::GenerateFWLink(ui->lineEdit->text(), ui->lineEdit_2->text(), url), ui->lineEdit_3->text());
    }
    ui->plainTextEdit->clear();
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
