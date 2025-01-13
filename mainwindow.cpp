#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("Project64 - Qt Interface");

    // Connect the Configuration menu action to the slot
    connect(ui->actionConfiguration, &QAction::triggered, this, &MainWindow::openConfigurationDialog);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openConfigurationDialog() {
    QDialog configDialog(this);
    Ui::Dialog configUi;
    configUi.setupUi(&configDialog);

    // Show the dialog
    configDialog.exec();
}
