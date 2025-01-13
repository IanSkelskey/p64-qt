#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/assets/icons/pj64.ico"));
    setWindowTitle("Project64");
}

MainWindow::~MainWindow()
{
    delete ui;
}
