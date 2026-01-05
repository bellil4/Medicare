#ifndef PATIENTREPOSITORY_H
#define PATIENTREPOSITORY_H

#include "models/patient.h"
#include <QList>
#include <QString>
#include <memory>

class PatientRepository {
public:
    static PatientRepository& instance();
    
    // CRUD operations
    bool create(Patient& patient, int currentUserId);
    std::unique_ptr<Patient> findById(int patientId, bool includeDeleted = false);
    QList<Patient> findAll(bool includeDeleted = false);
    QList<Patient> search(const QString& searchTerm, bool includeDeleted = false);
    bool update(const Patient& patient, int currentUserId);
    bool softDelete(int patientId, int currentUserId);
    bool restore(int patientId, int currentUserId);
    
    QString lastError() const;
    
    // Prevent copying
    PatientRepository(const PatientRepository&) = delete;
    PatientRepository& operator=(const PatientRepository&) = delete;
    
private:
    PatientRepository();
    ~PatientRepository();
    
    Patient mapQueryToPatient(class QSqlQuery& query);
    QString m_lastError;
};

#endif // PATIENTREPOSITORY_H
