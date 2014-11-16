#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>

#include <net/fwdownload.h>

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

    net::LoginData loginData;
    loginData.username = ui->lineEdit->text();
    loginData.password = ui->lineEdit_2->text();
    downloadManager = new net::DownloadManager(ui->spinBox->value(), getReloadSettings(), loginData);
    downloadTable = new model::DownloadTable(downloadManager);
    ui->tableView->setModel(downloadTable);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->verticalHeader()->setHidden(true);

    daemon->listen();
    timer.start(1000);

    connect(&timer, SIGNAL(timeout()), this, SLOT(displaySpeedSum()));
    connect(&timer, SIGNAL(timeout()), downloadTable, SLOT(refreshAll()));
    connect(daemon, SIGNAL(receivedLinks(QString)), ui->plainTextEdit, SLOT(appendPlainText(QString)));
    connect(ui->spinBox, SIGNAL(valueChanged(int)), downloadManager, SLOT(setParallelDownloads(int)));
    connect(ui->toolButton, SIGNAL(clicked()), this, SLOT(choosePath()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(addDownloads()));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateReloadSettings()));
    connect(ui->checkBox_2, SIGNAL(toggled(bool)), this, SLOT(updateReloadSettings()));
    connect(ui->checkBox_3, SIGNAL(toggled(bool)), this, SLOT(updateReloadSettings()));
    connect(ui->checkBox_4, SIGNAL(toggled(bool)), this, SLOT(updateReloadSettings()));
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(handleLoginData()));
    connect(ui->lineEdit_2, SIGNAL(textChanged(QString)), this, SLOT(handleLoginData()));
}

MainWindow::~MainWindow() {
    saveSettings();
    daemon->stop();
    delete daemon;
    delete downloadTable;
    delete ui;
}

void MainWindow::handleLoginData() {
    downloadManager->setLoginData(ui->lineEdit->text(), ui->lineEdit_2->text());
}

void MainWindow::updateReloadSettings() {
    downloadManager->setReloadSettings(getReloadSettings());
}

int MainWindow::getReloadSettings() {
    int reloadSettings = 0;
    if(ui->comboBox->currentIndex()==1)
        reloadSettings |= ReloadAfterRest;
    if(ui->checkBox_2->isChecked())
        reloadSettings |= ReloadFWError;
    if(ui->checkBox_3->isChecked())
        reloadSettings |= ReloadTimeout;
    if(ui->checkBox_4->isChecked())
        reloadSettings |= ReloadNetError;
    return reloadSettings;
}

void MainWindow::displaySpeedSum() {
    float speedSum = 0;
    for(int i=0; i<downloadManager->numberOfDownloads(); i++) {
        if(downloadManager->downloadAt(i).status == net::StatInProgress)
            speedSum += downloadManager->downloadAt(i).speed;
    }
    ui->statusBar->showMessage(model::DownloadTable::B2String(speedSum).append("/s"));
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
    if(QDir(ui->lineEdit_3->text()).exists()) {
        QStringList list = ui->plainTextEdit->toPlainText().split("\n");
        for(int i=0; i<list.length(); i++) {
            QString url = list.at(i).trimmed();
            if(!url.isEmpty())
                downloadManager->addLink(url, ui->lineEdit_3->text());
        }
        ui->plainTextEdit->clear();
        ui->tabWidget->setCurrentIndex(1);
    } else {
        QMessageBox::warning(this, "Incorrect Download Path", tr("The chosen path \"%1\" isn't existing.").arg(ui->lineEdit_3->text()));
    }
}

int MainWindow::parallelDownloads() const {
    return ui->spinBox->value();
}

void MainWindow::saveSettings() {
    QSettings settings("Codingspezis", "FDP");
    // login
    settings.setValue("username", ui->lineEdit->text());
    settings.setValue("password", ui->checkBox->checkState()?ui->lineEdit_2->text():"");
    settings.setValue("savepass", ui->checkBox->checkState());
    // download settings
    settings.setValue("path",     ui->lineEdit_3->text());
    settings.setValue("parallel", ui->spinBox->value());
    settings.setValue("reload-fwerror", ui->checkBox_2->checkState());
    settings.setValue("reload-timeout", ui->checkBox_3->checkState());
    settings.setValue("reload-neterror", ui->checkBox_4->checkState());
    settings.setValue("reloadorder", ui->comboBox->currentIndex());
    // window size
    settings.setValue("width", width());
    settings.setValue("height", height());
}

void MainWindow::loadSettings() {
    QSettings settings("Codingspezis", "FDP");
    // login
    ui->lineEdit->setText(settings.value("username").toString());
    ui->lineEdit_2->setText(settings.value("password").toString());
    ui->checkBox->setChecked(settings.value("savepass").toBool());
    // download settings
    ui->lineEdit_3->setText(settings.value("path").toString());
    ui->spinBox->setValue(settings.value("parallel").toInt());
    ui->checkBox_2->setChecked(settings.value("reload-fwerror").toBool());
    ui->checkBox_3->setChecked(settings.value("reload-timeout").toBool());
    ui->checkBox_4->setChecked(settings.value("reload-neterror").toBool());
    ui->comboBox->setCurrentIndex(settings.value("reloadorder").toInt());
    // window size
    resize(settings.value("width").toInt(), settings.value("height").toInt());
}

} // end of namespace gui
} // end of namespace fdp
