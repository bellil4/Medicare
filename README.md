# Medical Consultation Management System (Qt 6 + C++ + Oracle)

Production-grade desktop system for managing consultations, patients, staff, and audit logs.
Stack: Qt 6 Widgets (C++), Oracle SQL/PL-SQL, QOCI driver

This repository contains:
- Qt/C++ app scaffold (MVC-ish: models, repositories, controllers)
- Secure authentication (SHA-256)
- Database singleton using QOCI
- Audit logging for security & traceability
- Patient model + repository (soft delete)
- Oracle schema scripts for tables, sequences, and views

Getting started:
1. Install Qt6 and Oracle Instant Client (with SDK). Ensure QOCI driver is available in your Qt build.
2. Configure Oracle client and environment (LD_LIBRARY_PATH / PATH).
3. Build with CMake:
   mkdir build && cd build
   cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt6
   cmake --build .
4. Create the Oracle schema using `sql/schema.sql` and PL/SQL scripts in `sql/`.
5. Configure DB connection in `src/db/database.cpp` (or implement config file).
6. Run the app.

Security & compliance notes:
- Passwords hashed with SHA-256 + per-user salt.
- Soft delete for sensitive data.
- Immutable audit log entries written to an audit table (application-enforced).
- Parameterized queries (QSqlQuery bindValue) only — no inline SQL from UI.

Project roadmap:
- Milestone 1 (this scaffold): DB singleton, authentication, audit log, patient repo, light UI
- Milestone 2: Consultation lifecycle, attachments (PDF/images), PDF generator, email module
- Milestone 3: Reporting views, role-based UI rendering, session timeout & account lock policy
- Milestone 4: Polish, tests, packaging, documentation, screenshots

License: MIT (proposed)
