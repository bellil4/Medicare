#include "repositories/patientrepository.h"
#include "db/database.h"
#include "audit/auditlogger.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

PatientRepository::PatientRepository() {
}

PatientRepository::~PatientRepository() {
}

PatientRepository& PatientRepository::instance() {
    static PatientRepository instance;
    return instance;
}

bool PatientRepository::create(Patient& patient, int currentUserId) {
    if (!Database::instance().isConnected()) {
        m_lastError = "Database not connected";
        return false;
    }
    
    if (!patient.isValid()) {
        m_lastError = "Invalid patient data";
        return false;
    }
    
    QSqlDatabase db = Database::instance().getDatabase();
    QSqlQuery query(db);
    
    // Use parameterized query
    query.prepare("INSERT INTO patients "
                  "(patient_id, first_name, last_name, date_of_birth, gender, "
                  "phone, email, address, medical_history, is_deleted, "
                  "created_at, created_by, updated_at, updated_by) "
                  "VALUES "
                  "(patient_id_seq.NEXTVAL, :first_name, :last_name, :dob, :gender, "
                  ":phone, :email, :address, :history, 0, "
                  "CURRENT_TIMESTAMP, :created_by, CURRENT_TIMESTAMP, :updated_by)");
    
    query.bindValue(":first_name", patient.firstName());
    query.bindValue(":last_name", patient.lastName());
    query.bindValue(":dob", patient.dateOfBirth());
    query.bindValue(":gender", !patient.gender().isEmpty() ? QVariant(patient.gender()) : QVariant(QVariant::String));
    query.bindValue(":phone", !patient.phone().isEmpty() ? QVariant(patient.phone()) : QVariant(QVariant::String));
    query.bindValue(":email", !patient.email().isEmpty() ? QVariant(patient.email()) : QVariant(QVariant::String));
    query.bindValue(":address", !patient.address().isEmpty() ? QVariant(patient.address()) : QVariant(QVariant::String));
    query.bindValue(":history", !patient.medicalHistory().isEmpty() ? QVariant(patient.medicalHistory()) : QVariant(QVariant::String));
    query.bindValue(":created_by", currentUserId);
    query.bindValue(":updated_by", currentUserId);
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qCritical() << "Failed to create patient:" << m_lastError;
        return false;
    }
    
    // Get the generated patient ID
    // For Oracle, we need to query the sequence value
    QSqlQuery idQuery(db);
    idQuery.prepare("SELECT patient_id_seq.CURRVAL FROM dual");
    if (idQuery.exec() && idQuery.next()) {
        int patientId = idQuery.value(0).toInt();
        patient.setPatientId(patientId);
        
        // Log patient creation
        AuditLogger::instance().logAction(currentUserId, "PATIENT_CREATED", "patients", patientId,
                                         "", QString("Patient: %1 %2").arg(patient.firstName()).arg(patient.lastName()));
        
        m_lastError.clear();
        qInfo() << "Patient created successfully: ID" << patientId;
        return true;
    }
    
    m_lastError = "Failed to retrieve patient ID";
    return false;
}

std::unique_ptr<Patient> PatientRepository::findById(int patientId, bool includeDeleted) {
    if (!Database::instance().isConnected()) {
        m_lastError = "Database not connected";
        return nullptr;
    }
    
    QSqlDatabase db = Database::instance().getDatabase();
    QSqlQuery query(db);
    
    // Use parameterized query
    QString sql = "SELECT patient_id, first_name, last_name, date_of_birth, gender, "
                  "phone, email, address, medical_history, is_deleted, "
                  "created_at, created_by, updated_at, updated_by, deleted_at, deleted_by "
                  "FROM patients WHERE patient_id = :patient_id";
    
    if (!includeDeleted) {
        sql += " AND is_deleted = 0";
    }
    
    query.prepare(sql);
    query.bindValue(":patient_id", patientId);
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qCritical() << "Failed to find patient:" << m_lastError;
        return nullptr;
    }
    
    if (!query.next()) {
        m_lastError = "Patient not found";
        return nullptr;
    }
    
    auto patient = std::make_unique<Patient>(mapQueryToPatient(query));
    m_lastError.clear();
    return patient;
}

QList<Patient> PatientRepository::findAll(bool includeDeleted) {
    QList<Patient> patients;
    
    if (!Database::instance().isConnected()) {
        m_lastError = "Database not connected";
        return patients;
    }
    
    QSqlDatabase db = Database::instance().getDatabase();
    QSqlQuery query(db);
    
    QString sql = "SELECT patient_id, first_name, last_name, date_of_birth, gender, "
                  "phone, email, address, medical_history, is_deleted, "
                  "created_at, created_by, updated_at, updated_by, deleted_at, deleted_by "
                  "FROM patients";
    
    if (!includeDeleted) {
        sql += " WHERE is_deleted = 0";
    }
    
    sql += " ORDER BY last_name, first_name";
    
    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        qCritical() << "Failed to retrieve patients:" << m_lastError;
        return patients;
    }
    
    while (query.next()) {
        patients.append(mapQueryToPatient(query));
    }
    
    m_lastError.clear();
    return patients;
}

QList<Patient> PatientRepository::search(const QString& searchTerm, bool includeDeleted) {
    QList<Patient> patients;
    
    if (!Database::instance().isConnected()) {
        m_lastError = "Database not connected";
        return patients;
    }
    
    QSqlDatabase db = Database::instance().getDatabase();
    QSqlQuery query(db);
    
    // Use parameterized query for search
    QString sql = "SELECT patient_id, first_name, last_name, date_of_birth, gender, "
                  "phone, email, address, medical_history, is_deleted, "
                  "created_at, created_by, updated_at, updated_by, deleted_at, deleted_by "
                  "FROM patients "
                  "WHERE (UPPER(first_name) LIKE UPPER(:search) OR UPPER(last_name) LIKE UPPER(:search))";
    
    if (!includeDeleted) {
        sql += " AND is_deleted = 0";
    }
    
    sql += " ORDER BY last_name, first_name";
    
    query.prepare(sql);
    query.bindValue(":search", "%" + searchTerm + "%");
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qCritical() << "Failed to search patients:" << m_lastError;
        return patients;
    }
    
    while (query.next()) {
        patients.append(mapQueryToPatient(query));
    }
    
    m_lastError.clear();
    return patients;
}

bool PatientRepository::update(const Patient& patient, int currentUserId) {
    if (!Database::instance().isConnected()) {
        m_lastError = "Database not connected";
        return false;
    }
    
    if (!patient.isValid() || patient.patientId() <= 0) {
        m_lastError = "Invalid patient data";
        return false;
    }
    
    QSqlDatabase db = Database::instance().getDatabase();
    QSqlQuery query(db);
    
    // Use parameterized query
    query.prepare("UPDATE patients SET "
                  "first_name = :first_name, "
                  "last_name = :last_name, "
                  "date_of_birth = :dob, "
                  "gender = :gender, "
                  "phone = :phone, "
                  "email = :email, "
                  "address = :address, "
                  "medical_history = :history, "
                  "updated_at = CURRENT_TIMESTAMP, "
                  "updated_by = :updated_by "
                  "WHERE patient_id = :patient_id AND is_deleted = 0");
    
    query.bindValue(":first_name", patient.firstName());
    query.bindValue(":last_name", patient.lastName());
    query.bindValue(":dob", patient.dateOfBirth());
    query.bindValue(":gender", !patient.gender().isEmpty() ? QVariant(patient.gender()) : QVariant(QVariant::String));
    query.bindValue(":phone", !patient.phone().isEmpty() ? QVariant(patient.phone()) : QVariant(QVariant::String));
    query.bindValue(":email", !patient.email().isEmpty() ? QVariant(patient.email()) : QVariant(QVariant::String));
    query.bindValue(":address", !patient.address().isEmpty() ? QVariant(patient.address()) : QVariant(QVariant::String));
    query.bindValue(":history", !patient.medicalHistory().isEmpty() ? QVariant(patient.medicalHistory()) : QVariant(QVariant::String));
    query.bindValue(":updated_by", currentUserId);
    query.bindValue(":patient_id", patient.patientId());
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qCritical() << "Failed to update patient:" << m_lastError;
        return false;
    }
    
    if (query.numRowsAffected() == 0) {
        m_lastError = "Patient not found or already deleted";
        return false;
    }
    
    // Log patient update
    AuditLogger::instance().logAction(currentUserId, "PATIENT_UPDATED", "patients", patient.patientId(),
                                     "", QString("Patient: %1 %2").arg(patient.firstName()).arg(patient.lastName()));
    
    m_lastError.clear();
    qInfo() << "Patient updated successfully: ID" << patient.patientId();
    return true;
}

bool PatientRepository::softDelete(int patientId, int currentUserId) {
    if (!Database::instance().isConnected()) {
        m_lastError = "Database not connected";
        return false;
    }
    
    QSqlDatabase db = Database::instance().getDatabase();
    QSqlQuery query(db);
    
    // Use parameterized query for soft delete
    query.prepare("UPDATE patients SET "
                  "is_deleted = 1, "
                  "deleted_at = CURRENT_TIMESTAMP, "
                  "deleted_by = :deleted_by "
                  "WHERE patient_id = :patient_id AND is_deleted = 0");
    
    query.bindValue(":deleted_by", currentUserId);
    query.bindValue(":patient_id", patientId);
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qCritical() << "Failed to delete patient:" << m_lastError;
        return false;
    }
    
    if (query.numRowsAffected() == 0) {
        m_lastError = "Patient not found or already deleted";
        return false;
    }
    
    // Log patient deletion
    AuditLogger::instance().logAction(currentUserId, "PATIENT_DELETED", "patients", patientId);
    
    m_lastError.clear();
    qInfo() << "Patient soft-deleted successfully: ID" << patientId;
    return true;
}

bool PatientRepository::restore(int patientId, int currentUserId) {
    if (!Database::instance().isConnected()) {
        m_lastError = "Database not connected";
        return false;
    }
    
    QSqlDatabase db = Database::instance().getDatabase();
    QSqlQuery query(db);
    
    // Use parameterized query to restore
    query.prepare("UPDATE patients SET "
                  "is_deleted = 0, "
                  "deleted_at = NULL, "
                  "deleted_by = NULL, "
                  "updated_at = CURRENT_TIMESTAMP, "
                  "updated_by = :updated_by "
                  "WHERE patient_id = :patient_id AND is_deleted = 1");
    
    query.bindValue(":updated_by", currentUserId);
    query.bindValue(":patient_id", patientId);
    
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qCritical() << "Failed to restore patient:" << m_lastError;
        return false;
    }
    
    if (query.numRowsAffected() == 0) {
        m_lastError = "Patient not found or not deleted";
        return false;
    }
    
    // Log patient restoration
    AuditLogger::instance().logAction(currentUserId, "PATIENT_RESTORED", "patients", patientId);
    
    m_lastError.clear();
    qInfo() << "Patient restored successfully: ID" << patientId;
    return true;
}

QString PatientRepository::lastError() const {
    return m_lastError;
}

Patient PatientRepository::mapQueryToPatient(QSqlQuery& query) {
    Patient patient;
    
    patient.setPatientId(query.value("patient_id").toInt());
    patient.setFirstName(query.value("first_name").toString());
    patient.setLastName(query.value("last_name").toString());
    patient.setDateOfBirth(query.value("date_of_birth").toDate());
    patient.setGender(query.value("gender").toString());
    patient.setPhone(query.value("phone").toString());
    patient.setEmail(query.value("email").toString());
    patient.setAddress(query.value("address").toString());
    patient.setMedicalHistory(query.value("medical_history").toString());
    patient.setIsDeleted(query.value("is_deleted").toBool());
    patient.setCreatedAt(query.value("created_at").toDateTime());
    patient.setCreatedBy(query.value("created_by").toInt());
    patient.setUpdatedAt(query.value("updated_at").toDateTime());
    patient.setUpdatedBy(query.value("updated_by").toInt());
    
    if (!query.value("deleted_at").isNull()) {
        patient.setDeletedAt(query.value("deleted_at").toDateTime());
        patient.setDeletedBy(query.value("deleted_by").toInt());
    }
    
    return patient;
}
