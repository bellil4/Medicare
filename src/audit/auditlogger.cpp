#include "audit/auditlogger.h"
#include "db/database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

AuditLogger::AuditLogger() {
}

AuditLogger::~AuditLogger() {
}

AuditLogger& AuditLogger::instance() {
    static AuditLogger instance;
    return instance;
}

bool AuditLogger::logAction(int userId,
                            const QString& action,
                            const QString& tableName,
                            int recordId,
                            const QString& oldValues,
                            const QString& newValues,
                            const QString& ipAddress) {
    if (!Database::instance().isConnected()) {
        m_lastError = "Database not connected";
        qWarning() << "Audit log failed:" << m_lastError;
        return false;
    }
    
    QSqlDatabase db = Database::instance().getDatabase();
    QSqlQuery query(db);
    
    // Use parameterized query for security
    query.prepare("INSERT INTO audit_log "
                  "(audit_id, user_id, action, table_name, record_id, "
                  "old_values, new_values, ip_address, timestamp) "
                  "VALUES "
                  "(audit_log_id_seq.NEXTVAL, :user_id, :action, :table_name, :record_id, "
                  ":old_values, :new_values, :ip_address, CURRENT_TIMESTAMP)");
    
    query.bindValue(":user_id", userId > 0 ? QVariant(userId) : QVariant(QVariant::Int));
    query.bindValue(":action", action);
    query.bindValue(":table_name", !tableName.isEmpty() ? QVariant(tableName) : QVariant(QVariant::String));
    query.bindValue(":record_id", recordId > 0 ? QVariant(recordId) : QVariant(QVariant::Int));
    query.bindValue(":old_values", !oldValues.isEmpty() ? QVariant(oldValues) : QVariant(QVariant::String));
    query.bindValue(":new_values", !newValues.isEmpty() ? QVariant(newValues) : QVariant(QVariant::String));
    query.bindValue(":ip_address", !ipAddress.isEmpty() ? QVariant(ipAddress) : QVariant(QVariant::String));
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qCritical() << "Failed to insert audit log:" << m_lastError;
        return false;
    }
    
    m_lastError.clear();
    qDebug() << "Audit log recorded: User" << userId << "- Action:" << action;
    return true;
}

QString AuditLogger::lastError() const {
    return m_lastError;
}
