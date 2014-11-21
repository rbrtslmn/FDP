#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSortFilterProxyModel>

#include <net/httpdaemon.h>
#include <net/downloadmanager.h>
#include <model/downloadtable.h>

#include "reloadsettings.h"

namespace Ui {
    class MainWindow;
}

namespace fdp {
namespace gui {

class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int parallelDownloads() const;

protected slots:
    void choosePath();
    void addDownloads();
    void displaySpeedSum();
    void updateReloadSettings();
    void handleLoginData();
    void handleContextMenuRequest(const QPoint &pos);
    void handleSectionResize(int idx, int oldWidth, int newWidth);

protected:
    void saveSettings();
    void loadSettings();
    void loadColumnSizes();
    int getReloadSettings();
    void setupTable();

protected:
    Ui::MainWindow *ui;
    net::HTTPDaemon *daemon;
    fdp::net::DownloadManager *downloadManager;
    model::DownloadTable *downloadTable;
    QSortFilterProxyModel *sortProxyModel;
    QTimer timer;

};

} // end of namespace gui
} // end of namespace fdp

#endif // MAINWINDOW_H
