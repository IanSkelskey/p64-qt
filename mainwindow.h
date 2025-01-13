#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_config_dialog.h" // Include the dialog header
#include "ui_cover_downloader_dialog.h" // Include the generated header for the cover downloader

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openConfigurationDialog(); // Slot for opening the dialog
    void openCoverDownloader(); // Slot for opening the cover downloader

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
