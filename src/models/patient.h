#ifndef PATIENT_H
#define PATIENT_H

#include <QString>
#include <QDate>
#include <QDateTime>

class Patient {
public:
    Patient();
    
    // Getters
    int patientId() const;
    QString firstName() const;
    QString lastName() const;
    QDate dateOfBirth() const;
    QString gender() const;
    QString phone() const;
    QString email() const;
    QString address() const;
    QString medicalHistory() const;
    bool isDeleted() const;
    QDateTime createdAt() const;
    int createdBy() const;
    QDateTime updatedAt() const;
    int updatedBy() const;
    QDateTime deletedAt() const;
    int deletedBy() const;
    
    // Setters
    void setPatientId(int id);
    void setFirstName(const QString& firstName);
    void setLastName(const QString& lastName);
    void setDateOfBirth(const QDate& dob);
    void setGender(const QString& gender);
    void setPhone(const QString& phone);
    void setEmail(const QString& email);
    void setAddress(const QString& address);
    void setMedicalHistory(const QString& history);
    void setIsDeleted(bool deleted);
    void setCreatedAt(const QDateTime& dt);
    void setCreatedBy(int userId);
    void setUpdatedAt(const QDateTime& dt);
    void setUpdatedBy(int userId);
    void setDeletedAt(const QDateTime& dt);
    void setDeletedBy(int userId);
    
    // Utility methods
    QString fullName() const;
    int age() const;
    bool isValid() const;
    
private:
    int m_patientId;
    QString m_firstName;
    QString m_lastName;
    QDate m_dateOfBirth;
    QString m_gender;
    QString m_phone;
    QString m_email;
    QString m_address;
    QString m_medicalHistory;
    bool m_isDeleted;
    QDateTime m_createdAt;
    int m_createdBy;
    QDateTime m_updatedAt;
    int m_updatedBy;
    QDateTime m_deletedAt;
    int m_deletedBy;
};

#endif // PATIENT_H
