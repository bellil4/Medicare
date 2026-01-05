#include "db/database.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QDebug>

Database::Database() {}

Database::~Database() {
    close();
}

Database& Database::instance() {
    static Database singleton;
    return singleton;
}

bool Database::open(const QString& host,
                    const QString& service,
                    const QString& user,
                    const QString& password,
                    int port)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_db.isValid() && m_db.isOpen())
        return true;

    // QOCI uses connection string: host:port/service_name
    QString conn = QString("%1:%2/%3").arg(host).arg(port).arg(service);

    if (QSqlDatabase::contains("medical_conn"))
        m_db = QSqlDatabase::database("medical_conn");
    else
        m_db = QSqlDatabase::addDatabase("QOCI", "medical_conn");

    m_db.setHostName(host);
    m_db.setDatabaseName(conn);
    m_db.setUserName(user);
    m_db.setPassword(password);

    if (!m_db.open()) {
        qWarning() << "DB open error:" << m_db.lastError().text();
        return false;
    }
    return true;
}

QSqlDatabase Database::db() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_db;
}

bool Database::isOpen() const {
    return m_db.isValid() && m_db.isOpen();
}

void Database::close() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_db.isValid() && m_db.isOpen()) {
        m_db.close();
        QSqlDatabase::removeDatabase("medical_conn");
    }
}
