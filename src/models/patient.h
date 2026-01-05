#pragma once
#include <QString>
#include <QDate>
#include <QVariantMap>

class Patient {
public:
    int id = -1;
    QString firstName;
    QString lastName;
    QString nationalId;
    QString phone;
    QString email;
    QDate birthDate;
    QString bloodType;
    QString allergies;
    QString chronicConditions;
    QString address;
    bool isDeleted = false;

    QVariantMap toVariantMap() const;
    static Patient fromQueryRow(const QSqlRecord& rec);
};
