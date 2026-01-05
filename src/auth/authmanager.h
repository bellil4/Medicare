#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QString>
#include <QByteArray>

class AuthManager {
public:
    static AuthManager& instance();
    
    // Authenticate user with username and password
    bool authenticate(const QString& username, const QString& password);
    
    // Get current authenticated user information
    int currentUserId() const;
    QString currentUsername() const;
    int currentUserRoleId() const;
    bool isAuthenticated() const;
    
    // Logout current user
    void logout();
    
    // Password hashing utilities
    static QString generateSalt();
    static QString hashPassword(const QString& password, const QString& salt);
    
    QString lastError() const;
    
    // Prevent copying
    AuthManager(const AuthManager&) = delete;
    AuthManager& operator=(const AuthManager&) = delete;
    
private:
    AuthManager();
    ~AuthManager();
    
    bool updateLastLogin(int userId);
    
    int m_currentUserId;
    QString m_currentUsername;
    int m_currentUserRoleId;
    bool m_authenticated;
    QString m_lastError;
};

#endif // AUTHMANAGER_H
