#include "repositories/patientrepository.h"
#include "db/database.h"
#include "audit/auditlogger.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

bool PatientRepository::create(const Patient& p, int& outId) {
    if (!Database::instance().isOpen()) return false;
    QSqlQuery q(Database::instance().db());
    q.prepare(R"(
        INSERT INTO patients (first_name, last_name, national_id, phone, email, birth_date, blood_type, allergies, chronic_conditions, address)
        VALUES (:fn, :ln, :nid, :phone, :email, :bdate, :blood, :allergies, :chronic, :addr)
    )");
    q.bindValue(":fn", p.firstName);
    q.bindValue(":ln", p.lastName);
    q.bindValue(":nid", p.nationalId);
    q.bindValue(":phone", p.phone);
    q.bindValue(":email", p.email);
    q.bindValue(":bdate", p.birthDate);
    q.bindValue(":blood", p.bloodType);
    q.bindValue(":allergies", p.allergies);
    q.bindValue(":chronic", p.chronicConditions);
    q.bindValue(":addr", p.address);

    if (!q.exec()) {
        qWarning() << "Patient create failed:" << q.lastError().text();
        return false;
    }

    // Oracle: retrieve generated ID (if using identity) — here select max approximate for demo; replace with RETURNING if required
    QSqlQuery idq(Database::instance().db());
    idq.exec("SELECT patients_seq.CURRVAL FROM dual"); // if a sequence patients_seq exists
    if (idq.next()) outId = idq.value(0).toInt();
    else outId = -1;

    AuditLogger::instance().logEvent(QString::number(outId), QString(), "PATIENT_CREATED", "Patient", QString::number(outId), p.toVariantMap());
    return true;
}

std::optional<Patient> PatientRepository::getById(int id) {
    if (!Database::instance().isOpen()) return std::nullopt;
    QSqlQuery q(Database::instance().db());
    q.prepare("SELECT * FROM patients WHERE patient_id = :id AND is_deleted = 'N'");
    q.bindValue(":id", id);
    if (!q.exec()) {
        qWarning() << "Patient get failed:" << q.lastError().text();
        return std::nullopt;
    }
    if (!q.next()) return std::nullopt;
    return Patient::fromQueryRow(q.record());
}

QList<Patient> PatientRepository::search(const QString& term, int limit) {
    QList<Patient> res;
    if (!Database::instance().isOpen()) return res;
    QSqlQuery q(Database::instance().db());
    q.prepare(R"(
        SELECT * FROM patients
        WHERE is_deleted = 'N' AND
              (UPPER(first_name) LIKE UPPER(:t) OR UPPER(last_name) LIKE UPPER(:t) OR national_id LIKE :t)
        ORDER BY last_name FETCH FIRST :limit ROWS ONLY
    )");
    QString like = QString("%%1%").arg(term);
    q.bindValue(":t", like);
    q.bindValue(":limit", limit);
    if (!q.exec()) {
        qWarning() << "Patient search failed:" << q.lastError().text();
        return res;
    }
    while (q.next()) {
        res.append(Patient::fromQueryRow(q.record()));
    }
    return res;
}

bool PatientRepository::update(const Patient& p) {
    if (!Database::instance().isOpen()) return false;
    QSqlQuery q(Database::instance().db());
    q.prepare(R"(
        UPDATE patients SET first_name = :fn, last_name = :ln, national_id = :nid, phone = :phone, email = :email,
            birth_date = :bdate, blood_type = :blood, allergies = :allergies, chronic_conditions = :chronic,
            address = :addr, updated_at = SYSTIMESTAMP
        WHERE patient_id = :id
    )");
    q.bindValue(":fn", p.firstName);
    q.bindValue(":ln", p.lastName);
    q.bindValue(":nid", p.nationalId);
    q.bindValue(":phone", p.phone);
    q.bindValue(":email", p.email);
    q.bindValue(":bdate", p.birthDate);
    q.bindValue(":blood", p.bloodType);
    q.bindValue(":allergies", p.allergies);
    q.bindValue(":chronic", p.chronicConditions);
    q.bindValue(":addr", p.address);
    q.bindValue(":id", p.id);

    if (!q.exec()) {
        qWarning() << "Patient update failed:" << q.lastError().text();
        return false;
    }

    AuditLogger::instance().logEvent(QString::number(p.id), QString(), "PATIENT_UPDATED", "Patient", QString::number(p.id), p.toVariantMap());
    return true;
}

bool PatientRepository::softDelete(int id, int performedBy) {
    if (!Database::instance().isOpen()) return false;
    QSqlQuery q(Database::instance().db());
    q.prepare("UPDATE patients SET is_deleted = 'Y', updated_at = SYSTIMESTAMP WHERE patient_id = :id");
    q.bindValue(":id", id);
    if (!q.exec()) {
        qWarning() << "Patient soft-delete failed:" << q.lastError().text();
        return false;
    }
    AuditLogger::instance().logEvent(QString::number(performedBy), QString(), "PATIENT_SOFT_DELETED", "Patient", QString::number(id), {{"performedBy", performedBy}});
    return true;
}
