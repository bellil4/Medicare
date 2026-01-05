#include "auth/authmanager.h"
#include "db/database.h"
#include "audit/auditlogger.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QDebug>

AuthManager& AuthManager::instance() {
    static AuthManager inst;
    return inst;
}

QString AuthManager::generateSalt() const {
    QByteArray salt(32, '\0');
    for (int i = 0; i < salt.size(); ++i)
        salt[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    return QString::fromUtf8(salt.toBase64());
}

QString AuthManager::hashPassword(const QString& password, const QString& salt) const {
    QByteArray combined = password.toUtf8() + ":" + salt.toUtf8();
    QByteArray digest = QCryptographicHash::hash(combined, QCryptographicHash::Sha256);
    return QString::fromUtf8(digest.toHex());
}

std::optional<AuthResult> AuthManager::authenticate(const QString& username, const QString& password, const QString& remoteIp)
{
    if (!Database::instance().isOpen()) {
        return std::nullopt;
    }

    QSqlQuery q(Database::instance().db());
    q.prepare("SELECT user_id, password_hash, salt, role_id, is_locked, failed_attempts FROM users WHERE username = :username");
    q.bindValue(":username", username);
    if (!q.exec()) {
        qWarning() << "Auth query failed:" << q.lastError().text();
        return std::nullopt;
    }

    if (!q.next()) {
        AuditLogger::instance().logEvent(QString(), username, "LOGIN_FAILED_UNKNOWN_USER", "User", QString(), {{"remoteIp", remoteIp}});
        return AuthResult{false, "Invalid credentials", -1, username, QString()};
    }

    int userId = q.value("user_id").toInt();
    QString storedHash = q.value("password_hash").toString();
    QString salt = q.value("salt").toString();
    QString isLocked = q.value("is_locked").toString();
    int failedAttempts = q.value("failed_attempts").toInt();

    if (isLocked == "Y") {
        AuditLogger::instance().logEvent(QString::number(userId), username, "LOGIN_FAILED_ACCOUNT_LOCKED", "User", QString::number(userId), {{"remoteIp", remoteIp}});
        return AuthResult{false, "Account locked", userId, username, QString()};
    }

    QString computed = hashPassword(password, salt);
    if (computed != storedHash) {
        // increment failed attempts
        QSqlQuery inc(Database::instance().db());
        inc.prepare("UPDATE users SET failed_attempts = failed_attempts + 1 WHERE user_id = :uid");
        inc.bindValue(":uid", userId);
        inc.exec();
        AuditLogger::instance().logEvent(QString::number(userId), username, "LOGIN_FAILED_BAD_PASSWORD", "User", QString::number(userId), {{"remoteIp", remoteIp}});
        return AuthResult{false, "Invalid credentials", userId, username, QString()};
    }

    // successful: reset failed attempts, update last_login, get role
    QSqlQuery update(Database::instance().db());
    update.prepare("UPDATE users SET failed_attempts = 0, last_login = SYSTIMESTAMP WHERE user_id = :uid");
    update.bindValue(":uid", userId);
    update.exec();

    QSqlQuery r(Database::instance().db());
    r.prepare("SELECT r.role_name FROM roles r JOIN users u ON r.role_id = u.role_id WHERE u.user_id = :uid");
    r.bindValue(":uid", userId);
    r.exec();
    QString role;
    if (r.next()) role = r.value(0).toString();

    AuditLogger::instance().logEvent(QString::number(userId), username, "LOGIN_SUCCESS", "User", QString::number(userId), {{"remoteIp", remoteIp}});
    return AuthResult{true, "OK", userId, username, role};
}

bool AuthManager::changePassword(int userId, const QString& newPassword)
{
    if (!Database::instance().isOpen())
        return false;

    QString salt = generateSalt();
    QString newHash = hashPassword(newPassword, salt);

    QSqlQuery q(Database::instance().db());
    q.prepare("UPDATE users SET password_hash = :ph, salt = :s, updated_at = SYSTIMESTAMP WHERE user_id = :uid");
    q.bindValue(":ph", newHash);
    q.bindValue(":s", salt);
    q.bindValue(":uid", userId);
    if (!q.exec()) {
        qWarning() << "Failed to change password:" << q.lastError().text();
        return false;
    }

    AuditLogger::instance().logEvent(QString::number(userId), QString(), "PASSWORD_CHANGED", "User", QString::number(userId));
    return true;
}
