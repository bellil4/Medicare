#include "models/patient.h"
#include <QSqlRecord>
#include <QVariant>

QVariantMap Patient::toVariantMap() const {
    QVariantMap m;
    m["id"] = id;
    m["firstName"] = firstName;
    m["lastName"] = lastName;
    m["nationalId"] = nationalId;
    m["phone"] = phone;
    m["email"] = email;
    m["birthDate"] = birthDate;
    m["bloodType"] = bloodType;
    m["allergies"] = allergies;
    m["chronicConditions"] = chronicConditions;
    m["address"] = address;
    m["isDeleted"] = isDeleted;
    return m;
}

Patient Patient::fromQueryRow(const QSqlRecord& rec) {
    Patient p;
    p.id = rec.value("patient_id").toInt();
    p.firstName = rec.value("first_name").toString();
    p.lastName = rec.value("last_name").toString();
    p.nationalId = rec.value("national_id").toString();
    p.phone = rec.value("phone").toString();
    p.email = rec.value("email").toString();
    p.birthDate = rec.value("birth_date").toDate();
    p.bloodType = rec.value("blood_type").toString();
    p.allergies = rec.value("allergies").toString();
    p.chronicConditions = rec.value("chronic_conditions").toString();
    p.address = rec.value("address").toString();
    p.isDeleted = (rec.value("is_deleted").toString() == "Y");
    return p;
}
