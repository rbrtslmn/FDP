#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "httpdaemon.h"
#include "downloadtable.h"
#include "fwdownload.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void debug1(float Bps);

private:
    Ui::MainWindow *ui;
    DownloadTable *downloadTable;
    HTTPDaemon *daemon;
    FWDownload *debugDl;

};

#endif // MAINWINDOW_H
