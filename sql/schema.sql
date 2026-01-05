-- Medical Consultation Management System - Database Schema
-- Oracle Database Schema

-- Enable sequence for auto-increment IDs
-- Drop existing tables if they exist (for clean reinstall)
BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE audit_log CASCADE CONSTRAINTS';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE patients CASCADE CONSTRAINTS';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE users CASCADE CONSTRAINTS';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

BEGIN
   EXECUTE IMMEDIATE 'DROP TABLE roles CASCADE CONSTRAINTS';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

BEGIN
   EXECUTE IMMEDIATE 'DROP SEQUENCE user_id_seq';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

BEGIN
   EXECUTE IMMEDIATE 'DROP SEQUENCE patient_id_seq';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

BEGIN
   EXECUTE IMMEDIATE 'DROP SEQUENCE audit_log_id_seq';
EXCEPTION
   WHEN OTHERS THEN NULL;
END;
/

-- Create sequences
CREATE SEQUENCE user_id_seq START WITH 1 INCREMENT BY 1;
CREATE SEQUENCE patient_id_seq START WITH 1000 INCREMENT BY 1;
CREATE SEQUENCE audit_log_id_seq START WITH 1 INCREMENT BY 1;

-- Roles table
CREATE TABLE roles (
    role_id NUMBER(10) PRIMARY KEY,
    role_name VARCHAR2(50) NOT NULL UNIQUE,
    description VARCHAR2(255),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL
);

-- Users table
CREATE TABLE users (
    user_id NUMBER(10) PRIMARY KEY,
    username VARCHAR2(100) NOT NULL UNIQUE,
    password_hash VARCHAR2(64) NOT NULL,
    salt VARCHAR2(32) NOT NULL,
    role_id NUMBER(10) NOT NULL,
    full_name VARCHAR2(200) NOT NULL,
    email VARCHAR2(255),
    is_active NUMBER(1) DEFAULT 1 NOT NULL,
    last_login TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
    CONSTRAINT fk_users_role FOREIGN KEY (role_id) REFERENCES roles(role_id),
    CONSTRAINT chk_is_active CHECK (is_active IN (0, 1))
);

-- Patients table
CREATE TABLE patients (
    patient_id NUMBER(10) PRIMARY KEY,
    first_name VARCHAR2(100) NOT NULL,
    last_name VARCHAR2(100) NOT NULL,
    date_of_birth DATE NOT NULL,
    gender VARCHAR2(10),
    phone VARCHAR2(20),
    email VARCHAR2(255),
    address VARCHAR2(500),
    medical_history CLOB,
    is_deleted NUMBER(1) DEFAULT 0 NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
    created_by NUMBER(10),
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
    updated_by NUMBER(10),
    deleted_at TIMESTAMP,
    deleted_by NUMBER(10),
    CONSTRAINT fk_patients_created_by FOREIGN KEY (created_by) REFERENCES users(user_id),
    CONSTRAINT fk_patients_updated_by FOREIGN KEY (updated_by) REFERENCES users(user_id),
    CONSTRAINT fk_patients_deleted_by FOREIGN KEY (deleted_by) REFERENCES users(user_id),
    CONSTRAINT chk_is_deleted CHECK (is_deleted IN (0, 1)),
    CONSTRAINT chk_gender CHECK (gender IN ('Male', 'Female', 'Other'))
);

-- Audit Log table (immutable - no updates or deletes allowed)
CREATE TABLE audit_log (
    audit_id NUMBER(19) PRIMARY KEY,
    user_id NUMBER(10),
    action VARCHAR2(100) NOT NULL,
    table_name VARCHAR2(100),
    record_id NUMBER(19),
    old_values CLOB,
    new_values CLOB,
    ip_address VARCHAR2(45),
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
    CONSTRAINT fk_audit_user FOREIGN KEY (user_id) REFERENCES users(user_id)
);

-- Indexes for performance
CREATE INDEX idx_users_username ON users(username);
CREATE INDEX idx_users_role ON users(role_id);
CREATE INDEX idx_patients_name ON patients(last_name, first_name);
CREATE INDEX idx_patients_deleted ON patients(is_deleted);
CREATE INDEX idx_audit_user ON audit_log(user_id);
CREATE INDEX idx_audit_timestamp ON audit_log(timestamp);
CREATE INDEX idx_audit_table ON audit_log(table_name, record_id);

-- Insert default roles
INSERT INTO roles (role_id, role_name, description) VALUES (1, 'Admin', 'System Administrator with full access');
INSERT INTO roles (role_id, role_name, description) VALUES (2, 'Doctor', 'Medical doctor who can manage consultations');
INSERT INTO roles (role_id, role_name, description) VALUES (3, 'Nurse', 'Nurse with limited access');
INSERT INTO roles (role_id, role_name, description) VALUES (4, 'Receptionist', 'Front desk staff with patient management access');

COMMIT;

-- Display success message
SELECT 'Schema created successfully!' AS status FROM dual;
