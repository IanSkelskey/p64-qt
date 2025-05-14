#include "UI/mainwindow.h"
#include "UIAbstractionLayer.h"
#include <QApplication>
#include <QFile>

using namespace QT_UI;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Set application metadata
    QApplication::setApplicationName("Project64");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("Project64");
    QApplication::setOrganizationDomain("project64.org");
    
    // Initialize UI abstraction layer
    UIAbstractionLayer::instance().initialize();
    
    // Create and initialize main window
    MainWindow w;
    w.initialize();
    w.show();
    
    return a.exec();
}
