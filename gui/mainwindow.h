#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

protected:
    void saveSettings();
    void loadSettings();

protected:
    Ui::MainWindow *ui;
    model::DownloadTable *downloadTable;
    net::HTTPDaemon *daemon;
    net::DownloadManager *downloadManager;

};

} // end of namespace gui
} // end of namespace fdp

#endif // MAINWINDOW_H
