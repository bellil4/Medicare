#include "db/database.h"
#include <QSqlError>
#include <QMutexLocker>
#include <QDebug>

const QString Database::CONNECTION_NAME = "MedicalConsultationDB";

Database::Database() : m_connected(false) {
}

Database::~Database() {
    disconnect();
}

Database& Database::instance() {
    static Database instance;
    return instance;
}

bool Database::connect(const QString& hostname,
                       int port,
                       const QString& databaseName,
                       const QString& username,
                       const QString& password) {
    QMutexLocker locker(&m_mutex);
    
    if (m_connected) {
        qDebug() << "Database already connected";
        return true;
    }
    
    // Create Oracle database connection
    m_database = QSqlDatabase::addDatabase("QOCI", CONNECTION_NAME);
    m_database.setHostName(hostname);
    m_database.setPort(port);
    m_database.setDatabaseName(databaseName);
    m_database.setUserName(username);
    m_database.setPassword(password);
    
    if (!m_database.open()) {
        m_lastError = m_database.lastError().text();
        qCritical() << "Database connection failed:" << m_lastError;
        m_connected = false;
        return false;
    }
    
    m_connected = true;
    m_lastError.clear();
    qInfo() << "Database connected successfully";
    return true;
}

void Database::disconnect() {
    QMutexLocker locker(&m_mutex);
    
    if (m_connected && m_database.isOpen()) {
        m_database.close();
        m_connected = false;
        qInfo() << "Database disconnected";
    }
    
    if (QSqlDatabase::contains(CONNECTION_NAME)) {
        QSqlDatabase::removeDatabase(CONNECTION_NAME);
    }
}

bool Database::isConnected() const {
    QMutexLocker locker(&m_mutex);
    return m_connected && m_database.isOpen();
}

QSqlDatabase Database::getDatabase() const {
    QMutexLocker locker(&m_mutex);
    return QSqlDatabase::database(CONNECTION_NAME);
}

QString Database::lastError() const {
    QMutexLocker locker(&m_mutex);
    return m_lastError;
}
