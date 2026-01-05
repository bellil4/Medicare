#include "app.h"
#include "db/database.h"
#include <QDebug>

App::App(QObject* parent) : QObject(parent) {}

App::~App() {
    Database::instance().close();
}

bool App::init() {
    // Ideally load from config file; for scaffold, hard-coded or from env
    QString host = qEnvironmentVariable("MED_DB_HOST", "127.0.0.1");
    QString service = qEnvironmentVariable("MED_DB_SERVICE", "ORCLCDB");
    QString user = qEnvironmentVariable("MED_DB_USER", "med_app");
    QString password = qEnvironmentVariable("MED_DB_PASSWORD", "med_pass");
    bool ok = Database::instance().open(host, service, user, password);
    if (!ok) {
        qWarning() << "Failed to open DB connection";
        return false;
    }
    return true;
}
