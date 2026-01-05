#pragma once
#include "models/patient.h"
#include <QList>
#include <QString>

class PatientRepository {
public:
    PatientRepository() = default;
    bool create(const Patient& p, int& outId);
    std::optional<Patient> getById(int id);
    QList<Patient> search(const QString& term, int limit = 50);
    bool update(const Patient& p);
    bool softDelete(int id, int performedBy);
};
