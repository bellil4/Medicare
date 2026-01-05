#include "audit/auditlogger.h"
#include "db/database.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

AuditLogger& AuditLogger::instance() {
    static AuditLogger inst;
    return inst;
}

void AuditLogger::logEvent(const QString& userId,
                           const QString& username,
                           const QString& action,
                           const QString& objectType,
                           const QString& objectId,
                           const QVariantMap& details)
{
    if (!Database::instance().isOpen()) {
        qWarning() << "AuditLogger: DB not open";
        return;
    }

    QSqlQuery q(Database::instance().db());
    q.prepare(R"(
        INSERT INTO audit_log (user_id, username, action, object_type, object_id, details)
        VALUES (:user_id, :username, :action, :object_type, :object_id, :details)
    )");

    QJsonObject json = QJsonObject::fromVariantMap(details);
    QJsonDocument doc(json);
    QString detailsStr = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    q.bindValue(":user_id", userId.isEmpty() ? QVariant(QVariant::String) : userId);
    q.bindValue(":username", username);
    q.bindValue(":action", action);
    q.bindValue(":object_type", objectType);
    q.bindValue(":object_id", objectId);
    q.bindValue(":details", detailsStr);

    if (!q.exec()) {
        qWarning() << "Audit insert failed:" << q.lastError().text();
    }
}
