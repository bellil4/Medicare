#include "auth/authmanager.h"
#include "db/database.h"
#include "audit/auditlogger.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QVariant>
#include <QDateTime>
#include <QDebug>

AuthManager::AuthManager()
    : m_currentUserId(-1)
    , m_currentUserRoleId(-1)
    , m_authenticated(false) {
}

AuthManager::~AuthManager() {
}

AuthManager& AuthManager::instance() {
    static AuthManager instance;
    return instance;
}

bool AuthManager::authenticate(const QString& username, const QString& password) {
    if (!Database::instance().isConnected()) {
        m_lastError = "Database not connected";
        qWarning() << "Authentication failed:" << m_lastError;
        return false;
    }
    
    QSqlDatabase db = Database::instance().getDatabase();
    QSqlQuery query(db);
    
    // Use parameterized query to prevent SQL injection
    query.prepare("SELECT user_id, username, password_hash, salt, role_id, is_active "
                  "FROM users WHERE username = :username");
    query.bindValue(":username", username);
    
    if (!query.exec()) {
        m_lastError = "Database query failed: " + query.lastError().text();
        qCritical() << m_lastError;
        return false;
    }
    
    if (!query.next()) {
        m_lastError = "Invalid username or password";
        qWarning() << "Authentication failed for user:" << username;
        
        // Log failed login attempt (without user_id)
        AuditLogger::instance().logAction(-1, "LOGIN_FAILED", "users", -1,
                                         "", "username: " + username);
        return false;
    }
    
    int userId = query.value("user_id").toInt();
    QString storedHash = query.value("password_hash").toString();
    QString salt = query.value("salt").toString();
    int roleId = query.value("role_id").toInt();
    bool isActive = query.value("is_active").toBool();
    
    if (!isActive) {
        m_lastError = "User account is inactive";
        qWarning() << "Authentication failed: User inactive:" << username;
        
        // Log failed login attempt
        AuditLogger::instance().logAction(userId, "LOGIN_FAILED_INACTIVE", "users", userId);
        return false;
    }
    
    // Compute hash of provided password with stored salt
    QString computedHash = hashPassword(password, salt);
    
    if (computedHash != storedHash) {
        m_lastError = "Invalid username or password";
        qWarning() << "Authentication failed: Invalid password for user:" << username;
        
        // Log failed login attempt
        AuditLogger::instance().logAction(userId, "LOGIN_FAILED_PASSWORD", "users", userId);
        return false;
    }
    
    // Authentication successful
    m_currentUserId = userId;
    m_currentUsername = username;
    m_currentUserRoleId = roleId;
    m_authenticated = true;
    m_lastError.clear();
    
    // Update last login timestamp
    updateLastLogin(userId);
    
    // Log successful login
    AuditLogger::instance().logAction(userId, "LOGIN_SUCCESS", "users", userId);
    
    qInfo() << "User authenticated successfully:" << username;
    return true;
}

int AuthManager::currentUserId() const {
    return m_currentUserId;
}

QString AuthManager::currentUsername() const {
    return m_currentUsername;
}

int AuthManager::currentUserRoleId() const {
    return m_currentUserRoleId;
}

bool AuthManager::isAuthenticated() const {
    return m_authenticated;
}

void AuthManager::logout() {
    if (m_authenticated) {
        // Log logout action
        AuditLogger::instance().logAction(m_currentUserId, "LOGOUT", "users", m_currentUserId);
        
        qInfo() << "User logged out:" << m_currentUsername;
    }
    
    m_currentUserId = -1;
    m_currentUsername.clear();
    m_currentUserRoleId = -1;
    m_authenticated = false;
    m_lastError.clear();
}

QString AuthManager::generateSalt() {
    QByteArray salt;
    for (int i = 0; i < 16; ++i) {
        salt.append(static_cast<char>(QRandomGenerator::global()->bounded(256)));
    }
    return QString(salt.toHex());
}

QString AuthManager::hashPassword(const QString& password, const QString& salt) {
    QByteArray combined = password.toUtf8() + QByteArray::fromHex(salt.toUtf8());
    QByteArray hash = QCryptographicHash::hash(combined, QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

QString AuthManager::lastError() const {
    return m_lastError;
}

bool AuthManager::updateLastLogin(int userId) {
    QSqlDatabase db = Database::instance().getDatabase();
    QSqlQuery query(db);
    
    // Use parameterized query
    query.prepare("UPDATE users SET last_login = CURRENT_TIMESTAMP "
                  "WHERE user_id = :user_id");
    query.bindValue(":user_id", userId);
    
    if (!query.exec()) {
        qWarning() << "Failed to update last login:" << query.lastError().text();
        return false;
    }
    
    return true;
}
