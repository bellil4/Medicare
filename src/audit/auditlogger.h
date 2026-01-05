#ifndef AUDITLOGGER_H
#define AUDITLOGGER_H

#include <QString>
#include <QVariant>

class AuditLogger {
public:
    static AuditLogger& instance();
    
    bool logAction(int userId,
                   const QString& action,
                   const QString& tableName = QString(),
                   int recordId = -1,
                   const QString& oldValues = QString(),
                   const QString& newValues = QString(),
                   const QString& ipAddress = QString());
    
    QString lastError() const;
    
    // Prevent copying
    AuditLogger(const AuditLogger&) = delete;
    AuditLogger& operator=(const AuditLogger&) = delete;
    
private:
    AuditLogger();
    ~AuditLogger();
    
    QString m_lastError;
};

#endif // AUDITLOGGER_H
