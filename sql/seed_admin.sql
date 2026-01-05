-- Seed Admin User
-- This script inserts an initial administrator user into the database
-- 
-- IMPORTANT: Replace the placeholder hash and salt with values generated using one of these methods:
--
-- Method 1: Using OpenSSL (Linux/macOS)
-- -----------------------------------------
-- SALT=$(openssl rand -hex 16)
-- PASSWORD="AdminPassword123!"
-- HASH=$(echo -n "${PASSWORD}${SALT}" | openssl dgst -sha256 -hex | cut -d' ' -f2)
-- echo "Salt: $SALT"
-- echo "Hash: $HASH"
--
-- Method 2: Using Qt Console Application
-- ---------------------------------------
-- See README.md for a sample Qt program to generate hash and salt
--
-- Example values below (DO NOT USE IN PRODUCTION):
-- Password: AdminPassword123!
-- Salt: a1b2c3d4e5f6a7b8c9d0e1f2a3b4c5d6
-- Hash: Computed as SHA256(password + salt)

-- Insert Admin user
-- Replace COMPUTED_HASH and GENERATED_SALT with your actual values
INSERT INTO users (
    user_id,
    username,
    password_hash,
    salt,
    role_id,
    full_name,
    email,
    is_active
) VALUES (
    user_id_seq.NEXTVAL,
    'admin',
    'COMPUTED_HASH_REPLACE_ME_SEE_README',  -- Replace with your computed SHA-256 hash
    'GENERATED_SALT_REPLACE_ME_SEE_README',  -- Replace with your generated salt (32 char hex)
    1,  -- Admin role
    'System Administrator',
    'admin@medicare.local',
    1
);

COMMIT;

-- Display success message
SELECT 'Admin user created successfully!' AS status FROM dual;
SELECT 'Username: admin' AS info FROM dual;
SELECT 'IMPORTANT: Update password_hash and salt before using!' AS warning FROM dual;
