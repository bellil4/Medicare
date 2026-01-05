# Medical Consultation Management System

A secure, auditable, Oracle-backed MVC desktop application built with Qt6 C++ for managing medical consultations.

## Features

- **Secure Authentication**: SHA-256 password hashing with per-user salt
- **Audit Logging**: Immutable audit trail for all sensitive operations
- **Patient Management**: CRUD operations with soft delete support
- **Database Abstraction**: Singleton pattern for Oracle database connections via QOCI
- **MVC Architecture**: Clean separation of concerns

## Prerequisites

- **Qt 6.2+** with Widgets and Sql modules
- **Oracle Database** (11g or later) or Oracle Instant Client
- **CMake 3.16+**
- **C++17 compatible compiler** (GCC 7+, Clang 5+, MSVC 2017+)

## Database Setup

### 1. Create Oracle Database Schema

Execute the SQL schema to create required tables:

```bash
sqlplus username/password@database @sql/schema.sql
```

### 2. Generate Admin Password Hash

The application uses SHA-256 with per-user salt for password hashing. To generate a password hash for the admin user:

**Option A: Using OpenSSL (Linux/macOS)**

```bash
# Generate a random salt (hex string, 32 characters = 16 bytes)
SALT=$(openssl rand -hex 16)
echo "Salt: $SALT"

# Generate password hash (replace 'AdminPassword123!' with your desired password)
PASSWORD="AdminPassword123!"
# Convert salt from hex to binary, concatenate with password, then hash
HASH=$(printf "%s" "$PASSWORD" | cat - <(echo "$SALT" | xxd -r -p) | openssl dgst -sha256 -hex | cut -d' ' -f2)
echo "Hash: $HASH"
```

Note: The implementation concatenates the password (as UTF-8 bytes) with the salt (as decoded hex bytes) before hashing.

**Option B: Using Qt Console Tool (All platforms)**

You can create a small Qt console application to generate the hash:

```cpp
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QDebug>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    
    // Generate random salt
    QByteArray salt;
    for(int i = 0; i < 16; ++i) {
        salt.append(static_cast<char>(QRandomGenerator::global()->bounded(256)));
    }
    QString saltHex = salt.toHex();
    
    // Your password
    QString password = "AdminPassword123!";
    
    // Generate hash
    QByteArray combined = password.toUtf8() + QByteArray::fromHex(saltHex.toUtf8());
    QByteArray hash = QCryptographicHash::hash(combined, QCryptographicHash::Sha256);
    QString hashHex = hash.toHex();
    
    qDebug() << "Salt:" << saltHex;
    qDebug() << "Hash:" << hashHex;
    
    return 0;
}
```

### 3. Insert Admin User

Edit `sql/seed_admin.sql` with your generated hash and salt, then execute:

```bash
sqlplus username/password@database @sql/seed_admin.sql
```

## Building

### Configure and Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Run the Application

```bash
./MedicalConsultationSystem
```

## Database Configuration

Before running the application, ensure Oracle environment variables are set:

```bash
export ORACLE_HOME=/path/to/oracle
export LD_LIBRARY_PATH=$ORACLE_HOME/lib:$LD_LIBRARY_PATH
export TNS_ADMIN=/path/to/tnsnames
```

The application will prompt for database connection details on first run or you can modify the connection string in the source code.

## Architecture

```
src/
├── main.cpp                    # Application entry point
├── app.h/cpp                   # Application lifecycle management
├── db/
│   └── database.h/cpp          # Database singleton (QOCI)
├── audit/
│   └── auditlogger.h/cpp       # Audit logging system
├── auth/
│   └── authmanager.h/cpp       # Authentication with SHA-256
├── models/
│   └── patient.h/cpp           # Patient data model
├── repositories/
│   └── patientrepository.h/cpp # Patient data access (soft delete)
└── ui/
    └── mainwindow.h/cpp        # Main application window

sql/
├── schema.sql                  # Database schema definition
└── seed_admin.sql              # Admin user seeding script
```

## Security Features

- **Parameterized Queries**: All database operations use prepared statements with parameter binding
- **Password Security**: SHA-256 hashing with unique per-user salt (never stored in plain text)
- **Audit Trail**: All sensitive operations logged to immutable audit_log table
- **Soft Delete**: Patient records are never permanently deleted, only marked as deleted
- **No Inline SQL**: UI code never contains SQL strings

## Development Roadmap

**Phase A (Current)**: ✓ Database singleton, Auth, Audit, Patient repository, SQL schema

**Phase B (Planned)**: Consultation lifecycle management

**Phase C (Planned)**: PDF/Email generation and sending

**Phase D (Planned)**: Role-based UI rendering

**Phase E (Planned)**: Session timeouts and advanced security

## License

MIT License - see LICENSE file for details

## Support

For issues and questions, please use the GitHub issue tracker.
