#include <QApplication>
#include "ui/mainwindow.h"
#include "app.h"
#include <QDebug>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    App app;
    if (!app.init()) {
        qCritical() << "Application initialization failed (DB?)";
        return -1;
    }

    MainWindow w;
    w.setStatus("Connected (user: none, role: none)");
    w.show();
    return a.exec();
}
