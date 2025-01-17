#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Connect the Configuration menu action to the slot
    connect(ui->actionConfiguration, &QAction::triggered, this, &MainWindow::openConfigurationDialog);
    connect(ui->actionCover_Downloader, &QAction::triggered, this, &MainWindow::openCoverDownloader);
    connect(ui->actionAbout_Project64, &QAction::triggered, this, &MainWindow::openAboutDialog); // Connect the About menu action to the slot
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openConfigurationDialog() {
    QDialog configDialog(this);
    Ui::ConfigDialog configUi;
    configUi.setupUi(&configDialog);

    // Show the dialog
    configDialog.exec();
}

void MainWindow::openCoverDownloader() {
    QDialog coverDialog(this);
    Ui::CoverDownloaderDialog coverUi;
    coverUi.setupUi(&coverDialog);

    // Show the dialog
    coverDialog.exec();
}

void MainWindow::openAboutDialog() {
    AboutDialog aboutDialog(this);
    aboutDialog.exec();
}