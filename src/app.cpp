#include "app.h"
#include "ui/mainwindow.h"
#include "db/database.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

App::App(QObject *parent)
    : QObject(parent)
    , m_mainWindow(nullptr)
    , m_initialized(false) {
}

App::~App() {
    shutdown();
}

bool App::initialize() {
    if (m_initialized) {
        qWarning() << "Application already initialized";
        return true;
    }
    
    qInfo() << "Initializing Medical Consultation Management System...";
    
    // Create main window
    m_mainWindow = new MainWindow();
    
    // Attempt to connect to database
    if (!connectToDatabase()) {
        m_mainWindow->updateDatabaseStatus(false, "Failed to connect to database");
        qWarning() << "Database connection failed, but application will continue";
        // Don't return false - allow the application to run without DB for initial setup
    } else {
        m_mainWindow->updateDatabaseStatus(true, "Database connected successfully");
    }
    
    m_initialized = true;
    qInfo() << "Application initialized successfully";
    return true;
}

void App::run() {
    if (!m_initialized) {
        qCritical() << "Application not initialized";
        return;
    }
    
    showMainWindow();
}

void App::shutdown() {
    if (!m_initialized) {
        return;
    }
    
    qInfo() << "Shutting down application...";
    
    // Disconnect from database
    if (Database::instance().isConnected()) {
        Database::instance().disconnect();
    }
    
    // Clean up main window
    if (m_mainWindow) {
        delete m_mainWindow;
        m_mainWindow = nullptr;
    }
    
    m_initialized = false;
    qInfo() << "Application shutdown complete";
}

bool App::connectToDatabase() {
    // In a production environment, these would be read from a configuration file
    // or environment variables. For this scaffold, we'll use a simple dialog.
    
    bool ok;
    
    QString hostname = QInputDialog::getText(nullptr, "Database Configuration",
                                            "Oracle Hostname (e.g., localhost):",
                                            QLineEdit::Normal,
                                            "localhost", &ok);
    if (!ok || hostname.isEmpty()) {
        qWarning() << "Database configuration cancelled";
        return false;
    }
    
    int port = QInputDialog::getInt(nullptr, "Database Configuration",
                                   "Oracle Port:",
                                   1521, 1, 65535, 1, &ok);
    if (!ok) {
        qWarning() << "Database configuration cancelled";
        return false;
    }
    
    QString databaseName = QInputDialog::getText(nullptr, "Database Configuration",
                                                "Database Name (Service Name/SID):",
                                                QLineEdit::Normal,
                                                "ORCL", &ok);
    if (!ok || databaseName.isEmpty()) {
        qWarning() << "Database configuration cancelled";
        return false;
    }
    
    QString username = QInputDialog::getText(nullptr, "Database Configuration",
                                            "Database Username:",
                                            QLineEdit::Normal,
                                            "", &ok);
    if (!ok || username.isEmpty()) {
        qWarning() << "Database configuration cancelled";
        return false;
    }
    
    QString password = QInputDialog::getText(nullptr, "Database Configuration",
                                            "Database Password:",
                                            QLineEdit::Password,
                                            "", &ok);
    if (!ok) {
        qWarning() << "Database configuration cancelled";
        return false;
    }
    
    // Attempt connection
    bool connected = Database::instance().connect(hostname, port, databaseName, username, password);
    
    if (!connected) {
        QString error = Database::instance().lastError();
        qCritical() << "Database connection failed:" << error;
        
        QMessageBox::critical(nullptr, "Database Connection Failed",
                            QString("Failed to connect to database:\n\n%1\n\n"
                                  "Please ensure:\n"
                                  "- Oracle database is running\n"
                                  "- QOCI driver is installed\n"
                                  "- Oracle client libraries are in PATH\n"
                                  "- Connection details are correct").arg(error));
        return false;
    }
    
    qInfo() << "Connected to Oracle database successfully";
    return true;
}

void App::showMainWindow() {
    if (m_mainWindow) {
        m_mainWindow->show();
    }
}
