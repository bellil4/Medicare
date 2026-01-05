#include "ui/mainwindow.h"
#include <QStatusBar>
#include <QLabel>
#include <QMenuBar>
#include <QAction>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    initUi();
}

void MainWindow::initUi() {
    setWindowTitle("Medical Consultation Management System");
    // Menu
    QMenuBar* mb = menuBar();
    QMenu* fileMenu = mb->addMenu("&File");
    QAction* exitAct = fileMenu->addAction("E&xit");
    connect(exitAct, &QAction::triggered, this, &MainWindow::close);

    // Status bar
    statusBar()->showMessage("Not connected");
}

void MainWindow::setStatus(const QString& text) {
    statusBar()->showMessage(text);
}
