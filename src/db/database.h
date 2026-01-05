#pragma once
#include <QtSql/QSqlDatabase>
#include <QString>
#include <mutex>

class Database {
public:
    static Database& instance();
    bool open(const QString& host,
              const QString& service,
              const QString& user,
              const QString& password,
              int port = 1521);
    QSqlDatabase db();
    bool isOpen() const;
    void close();

private:
    Database();
    ~Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    QSqlDatabase m_db;
    mutable std::mutex m_mutex;
};
