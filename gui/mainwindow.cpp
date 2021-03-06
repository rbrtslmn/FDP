#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QMenu>
#include <QDesktopServices>

#include <net/fwdownload.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace fdp {
namespace gui {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    daemon(new net::HTTPDaemon()),
    sortProxyModel(new QSortFilterProxyModel())
{
    // set up ui
    ui->setupUi(this);
    setWindowTitle("Free-Way.me Download Program");
    setWindowIcon(QIcon(":/symbols/icon.png"));
    loadSettings();
    // set up download manager
    net::LoginData loginData;
    loginData.username = ui->lineEdit->text();
    loginData.password = ui->lineEdit_2->text();
    downloadManager = new net::DownloadManager(ui->spinBox->value(), getReloadSettings(), loginData);
    // set up download table
    setupTable();
    // set up http daemon
    if(!daemon->listen())
        QMessageBox::warning(this, "Socket Error",
            "Couldn't set up CNL socket (only manual link adding supported). "
            "This can be caused by running multiple instances of this application.");
    timer.start(1000);
    // set up connections
    setupConnections();
}

MainWindow::~MainWindow() {
    saveSettings();
    daemon->stop();
    delete downloadTable;
    delete downloadManager;
    delete sortProxyModel;
    delete daemon;
    delete ui;
}

void MainWindow::setupConnections() {
    connect(&timer, SIGNAL(timeout()), this, SLOT(tickTack()));
    connect(daemon, SIGNAL(receivedLinks(QString)), this, SLOT(handleReceivedLinks(QString)));
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(handleContextMenuRequest(QPoint)));
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

void MainWindow::tickTack() {
    displaySpeedSum();
    downloadTable->dataChanged(downloadTable->index(0, 0), downloadTable->index(downloadManager->numberOfDownloads() - 1, 7));
}

void MainWindow::handleReceivedLinks(QString links) {
    ui->plainTextEdit->appendPlainText(links);
    ui->tabWidget->setCurrentIndex(0);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if(event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->key() == Qt::Key_Delete) {
            if(sureDelete()) {
                QList<int> selection = descendingTableSelection();
                for(int i=0; i<selection.length(); i++)
                    handleContextMenuChoice("Delete", selection.at(i));
            }
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

void MainWindow::setupTable() {
    // set up model
    downloadTable = new model::DownloadTable(downloadManager);
    sortProxyModel->setSortRole(Qt::UserRole);
    sortProxyModel->setDynamicSortFilter(true);
    sortProxyModel->setSourceModel(downloadTable);
    ui->tableView->setModel(sortProxyModel);
    // connection before setting section size
    connect(ui->tableView->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(handleSectionResize(int,int,int)));
    // table settings
    ui->tableView->installEventFilter(this);
    ui->tableView->setSortingEnabled(true);
    ui->tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setWordWrap(false);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->verticalHeader()->setHidden(true);
    loadTableSettings();
}

QList<int> MainWindow::descendingTableSelection() {
    QList<int> returnValue;
    QModelIndexList selectedDownloads = ui->tableView->selectionModel()->selectedRows();
    // search for index of download table from indexes of proxy model
    // this is needed because the removing of rows has to be done in an index descending order
    for(int i=downloadManager->numberOfDownloads()-1; i>=0; i--) {
        int idx = sortProxyModel->mapFromSource(downloadTable->index(i, 0)).row();
        for(int j=0; j<selectedDownloads.length(); j++) {
            if(idx == selectedDownloads.at(j).row()) {
                returnValue.append(i);
                break;
            }
        }
    }
    return returnValue;
}

void MainWindow::handleSectionResize(int idx, int oldWidth, int newWidth) {
    (void)oldWidth;
    if(idx == 3)
        downloadTable->setProgressColumnWidth(newWidth);
}

bool MainWindow::sureDelete() {
    QList<int> selectedDownloads = descendingTableSelection();
    for(int i=0; i<selectedDownloads.length(); i++) {
        net::DownloadStatus status = downloadManager->downloadAt(selectedDownloads.at(i)).status;
        if(status == net::StatPending || status == net::StatAbout2Start || status == net::StatInProgress) {
            int choice = QMessageBox::warning(this, "Are you sure?",
                "One or more selected downloads are not finished yet.\n"
                "Delete downloads anyway?", QMessageBox::Yes, QMessageBox::No);
            return choice == QMessageBox::Yes;
        }
    }
    return true;
}

bool MainWindow::sureRestart() {
    QList<int> selectedDownloads = descendingTableSelection();
    for(int i=0; i<selectedDownloads.length(); i++) {
        net::DownloadInformation info = downloadManager->downloadAt(selectedDownloads.at(i));
        if(!info.file.isEmpty() && QFile(tr("%1%2").arg(info.path).arg(info.file)).exists()) {
            int choice = QMessageBox::warning(this, "Are you sure?",
                "For one or more selected downloads there already exist files.\n"
                "Delete files and restart downloads anyway?", QMessageBox::Yes, QMessageBox::No);
            return choice == QMessageBox::Yes;
        }
    }
    return true;
}

void MainWindow::handleContextMenuRequest(const QPoint &pos) {
    QPoint globalPos = ui->tableView->viewport()->mapToGlobal(pos);
    QList<int> selectedDownloads = descendingTableSelection();
    // if there are selected rows
    if(!selectedDownloads.isEmpty()) {
        QMenu contextMenu;
        contextMenu.addAction(QIcon(":/symbols/folder-g.png"), "Open Directory");
        contextMenu.addAction(QIcon(":/symbols/stop-g.png"), "Stop");
        contextMenu.addAction(QIcon(":/symbols/restart-g.png"), "Restart");
        contextMenu.addAction(QIcon(":/symbols/delete-g.png"), "Delete");
        QAction* selectedItem = contextMenu.exec(globalPos);
        // if an action was clicked
        if(selectedItem) {
            // ask before deleting downloads
            if(selectedItem->text() == "Delete" && !sureDelete())
                return;
            // ask before restarting downloads
            if(selectedItem->text() == "Restart" && !sureRestart())
                return;
            QStringList directories;
            // row index decreases with increasing array index
            for(int i=0; i<selectedDownloads.length(); i++) {
                if(selectedItem->text() == "Open Directory") {
                    if(!directories.contains(downloadManager->downloadAt(selectedDownloads.at(i)).path))
                        directories.append(downloadManager->downloadAt(selectedDownloads.at(i)).path);
                } else handleContextMenuChoice(selectedItem->text(), selectedDownloads.at(i));
            }
            // open directories
            for(int i=0; i<directories.length(); i++)
                QDesktopServices::openUrl(directories.at(i));
        }
    }
}

void MainWindow::handleContextMenuChoice(QString text, int i) {
    if(text == "Stop") {
        downloadManager->stopDownload(i);
    } else if(text == "Restart") {
        downloadManager->restartDownload(i);
    } else if(text == "Delete") {
        downloadTable->beginDelete(i);
        downloadManager->deleteDownload(i);
        downloadTable->endDelete();
    }
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
    int progressSum = 0;
    for(int i=0; i<downloadManager->numberOfDownloads(); i++) {
        if(downloadManager->downloadAt(i).status == net::StatInProgress) {
            speedSum += downloadManager->downloadAt(i).speed;
            progressSum++;
        }
    }
    ui->statusBar->showMessage(
        tr(progressSum==1?"%1 active download: %2":"%1 active downloads: %2")
        .arg(progressSum)
        .arg(model::DownloadTable::B2String(speedSum).append("/s")));
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
            if(!url.isEmpty()) {
                downloadTable->beginInsert(downloadManager->numberOfDownloads());
                downloadManager->addLink(url, ui->lineEdit_3->text());
                downloadTable->endInsert();
            }
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
    // tab
    settings.setValue("active-tab", ui->tabWidget->currentIndex());
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
    // table column size
    for(int i=0; i<8; i++)
        settings.setValue(tr("column-width-%1").arg(i), ui->tableView->horizontalHeader()->sectionSize(i));
    // sorting
    settings.setValue("sort-index", ui->tableView->horizontalHeader()->sortIndicatorSection());
    int order = ui->tableView->horizontalHeader()->sortIndicatorOrder()==Qt::AscendingOrder?0:1;
    settings.setValue("sort-order", order);
    // table section postions
    for(int i=0; i<8; i++)
        settings.setValue(tr("column-position-%1").arg(i), ui->tableView->horizontalHeader()->logicalIndex(i));
    // settings indicator
    settings.setValue("saved", true);
}

void MainWindow::loadSettings() {
    QSettings settings("Codingspezis", "FDP");
    if(settings.value("saved").toBool()) {
        // login
        ui->lineEdit->setText(settings.value("username").toString());
        ui->lineEdit_2->setText(settings.value("password").toString());
        ui->checkBox->setChecked(settings.value("savepass").toBool());
        // tab
        ui->tabWidget->setCurrentIndex(settings.value("active-tab").toInt());
        // download settings
        ui->lineEdit_3->setText(settings.value("path").toString());
        ui->spinBox->setValue(settings.value("parallel").toInt());
        ui->checkBox_2->setChecked(settings.value("reload-fwerror").toBool());
        ui->checkBox_3->setChecked(settings.value("reload-timeout").toBool());
        ui->checkBox_4->setChecked(settings.value("reload-neterror").toBool());
        ui->comboBox->setCurrentIndex(settings.value("reloadorder").toInt());
        // window size
        resize(settings.value("width").toInt(), settings.value("height").toInt());
    } else {
        downloadTable->setProgressColumnWidth(ui->tableView->horizontalHeader()->sectionSize(3));
    }
}

void MainWindow::loadTableSettings() {
    QSettings settings("Codingspezis", "FDP");
    if(settings.value("saved").toBool()) {
        // column sizes
        for(int i=0; i<8; i++) {
            int width = settings.value(tr("column-width-%1").arg(i)).toInt();
            if(width > 0)
                ui->tableView->horizontalHeader()->resizeSection(i, width);
        }
        // sorting
        Qt::SortOrder order = settings.value("sort-order").toInt()==1?Qt::DescendingOrder:Qt::AscendingOrder;
        ui->tableView->horizontalHeader()->setSortIndicator(settings.value("sort-index").toInt(), order);
        // table section postions
        for(int i=0; i<8; i++)
            ui->tableView->horizontalHeader()->moveSection(
                ui->tableView->horizontalHeader()->visualIndex(settings.value(tr("column-position-%1").arg(i)).toInt()), i);
    }
}

} // end of namespace gui
} // end of namespace fdp
