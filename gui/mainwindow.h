#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <net/httpdaemon.h>
#include <net/downloadmanager.h>
#include <model/downloadtable.h>

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

protected slots:
    void choosePath();
    void addDownloads();
    void displaySpeedSum();

protected:
    void saveSettings();
    void loadSettings();

protected:
    Ui::MainWindow *ui;
    model::DownloadTable *downloadTable;
    net::HTTPDaemon *daemon;
    net::DownloadManager *downloadManager;
    QTimer timer;

};

} // end of namespace gui
} // end of namespace fdp

#endif // MAINWINDOW_H
