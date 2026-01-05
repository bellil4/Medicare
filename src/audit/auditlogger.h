#pragma once
#include <QString>
#include <QVariantMap>

class AuditLogger {
public:
    static AuditLogger& instance();
    void logEvent(const QString& userId,
                  const QString& username,
                  const QString& action,
                  const QString& objectType = QString(),
                  const QString& objectId = QString(),
                  const QVariantMap& details = QVariantMap());
private:
    AuditLogger() = default;
    AuditLogger(const AuditLogger&) = delete;
    AuditLogger& operator=(const AuditLogger&) = delete;
    ~AuditLogger() = default;
};
