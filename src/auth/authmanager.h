#pragma once
#include <QString>
#include <optional>

struct AuthResult {
    bool success;
    QString message;
    int userId;
    QString username;
    QString role;
};

class AuthManager {
public:
    static AuthManager& instance();
    std::optional<AuthResult> authenticate(const QString& username, const QString& password, const QString& remoteIp = QString());
    bool changePassword(int userId, const QString& newPassword);
    // Additional: lock/unlock, create user etc.
private:
    AuthManager() = default;
    ~AuthManager() = default;
    AuthManager(const AuthManager&) = delete;
    AuthManager& operator=(const AuthManager&) = delete;

    QString hashPassword(const QString& password, const QString& salt) const;
    QString generateSalt() const;
};
