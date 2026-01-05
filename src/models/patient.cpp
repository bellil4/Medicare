#include "models/patient.h"

Patient::Patient()
    : m_patientId(-1)
    , m_isDeleted(false)
    , m_createdBy(-1)
    , m_updatedBy(-1)
    , m_deletedBy(-1) {
}

// Getters
int Patient::patientId() const {
    return m_patientId;
}

QString Patient::firstName() const {
    return m_firstName;
}

QString Patient::lastName() const {
    return m_lastName;
}

QDate Patient::dateOfBirth() const {
    return m_dateOfBirth;
}

QString Patient::gender() const {
    return m_gender;
}

QString Patient::phone() const {
    return m_phone;
}

QString Patient::email() const {
    return m_email;
}

QString Patient::address() const {
    return m_address;
}

QString Patient::medicalHistory() const {
    return m_medicalHistory;
}

bool Patient::isDeleted() const {
    return m_isDeleted;
}

QDateTime Patient::createdAt() const {
    return m_createdAt;
}

int Patient::createdBy() const {
    return m_createdBy;
}

QDateTime Patient::updatedAt() const {
    return m_updatedAt;
}

int Patient::updatedBy() const {
    return m_updatedBy;
}

QDateTime Patient::deletedAt() const {
    return m_deletedAt;
}

int Patient::deletedBy() const {
    return m_deletedBy;
}

// Setters
void Patient::setPatientId(int id) {
    m_patientId = id;
}

void Patient::setFirstName(const QString& firstName) {
    m_firstName = firstName;
}

void Patient::setLastName(const QString& lastName) {
    m_lastName = lastName;
}

void Patient::setDateOfBirth(const QDate& dob) {
    m_dateOfBirth = dob;
}

void Patient::setGender(const QString& gender) {
    m_gender = gender;
}

void Patient::setPhone(const QString& phone) {
    m_phone = phone;
}

void Patient::setEmail(const QString& email) {
    m_email = email;
}

void Patient::setAddress(const QString& address) {
    m_address = address;
}

void Patient::setMedicalHistory(const QString& history) {
    m_medicalHistory = history;
}

void Patient::setIsDeleted(bool deleted) {
    m_isDeleted = deleted;
}

void Patient::setCreatedAt(const QDateTime& dt) {
    m_createdAt = dt;
}

void Patient::setCreatedBy(int userId) {
    m_createdBy = userId;
}

void Patient::setUpdatedAt(const QDateTime& dt) {
    m_updatedAt = dt;
}

void Patient::setUpdatedBy(int userId) {
    m_updatedBy = userId;
}

void Patient::setDeletedAt(const QDateTime& dt) {
    m_deletedAt = dt;
}

void Patient::setDeletedBy(int userId) {
    m_deletedBy = userId;
}

// Utility methods
QString Patient::fullName() const {
    return m_firstName + " " + m_lastName;
}

int Patient::age() const {
    if (!m_dateOfBirth.isValid()) {
        return -1;
    }
    
    QDate today = QDate::currentDate();
    int age = today.year() - m_dateOfBirth.year();
    
    // Adjust if birthday hasn't occurred yet this year
    if (today.month() < m_dateOfBirth.month() ||
        (today.month() == m_dateOfBirth.month() && today.day() < m_dateOfBirth.day())) {
        age--;
    }
    
    return age;
}

bool Patient::isValid() const {
    return !m_firstName.isEmpty() && 
           !m_lastName.isEmpty() && 
           m_dateOfBirth.isValid();
}
