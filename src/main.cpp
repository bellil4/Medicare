#include "app.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[]) {
    QApplication application(argc, argv);
    
    // Set application metadata
    QApplication::setApplicationName("Medical Consultation Management System");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("Medicare");
    QApplication::setOrganizationDomain("medicare.local");
    
    qInfo() << "Starting Medical Consultation Management System v1.0.0";
    qInfo() << "Qt Version:" << qVersion();
    
    // Create and initialize application
    App app;
    
    if (!app.initialize()) {
        qCritical() << "Failed to initialize application";
        return 1;
    }
    
    // Run application
    app.run();
    
    // Enter event loop
    int result = application.exec();
    
    // Shutdown
    app.shutdown();
    
    qInfo() << "Application exited with code:" << result;
    return result;
}
