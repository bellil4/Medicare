#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QString>
#include <QMutex>

class Database {
public:
    static Database& instance();
    
    bool connect(const QString& hostname, 
                 int port,
                 const QString& databaseName,
                 const QString& username,
                 const QString& password);
    
    void disconnect();
    bool isConnected() const;
    QSqlDatabase getDatabase() const;
    QString lastError() const;
    
    // Prevent copying
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    
private:
    Database();
    ~Database();
    
    QSqlDatabase m_database;
    bool m_connected;
    QString m_lastError;
    mutable QMutex m_mutex;
    static const QString CONNECTION_NAME;
};

#endif // DATABASE_H
