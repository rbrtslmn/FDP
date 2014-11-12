#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <net/httpdaemon.h>
#include <net/fwdownload.h>
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
    void debug1(float Bps);

private:
    Ui::MainWindow *ui;
    model::DownloadTable *downloadTable;
    net::HTTPDaemon *daemon;
    net::FWDownload *debugDl;

};

} // end of namespace gui
} // end of namespace fdp

#endif // MAINWINDOW_H
