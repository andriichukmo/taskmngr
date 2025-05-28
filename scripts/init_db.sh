#!/usr/bin/env bash
set -euo pipefail

DB_NAME="${DB_NAME:-tasks}"
DB_USER="${DB_USER:-tasks_user}"
DB_PASS="${DB_PASS:-tasks_pass}"

# Проверяем, что psql установлен
if ! command -v psql >/dev/null 2>&1; then
  echo "Error: psql not found. Please install the PostgreSQL client tools." >&2
  exit 1
fi

# 1) База
echo "Ensuring database \"$DB_NAME\" exists…"
if ! sudo -u postgres psql -tAc "SELECT 1 FROM pg_database WHERE datname='${DB_NAME}'" | grep -q 1; then
  echo "➤ Creating database \"$DB_NAME\"…"
  sudo -u postgres psql -v ON_ERROR_STOP=1 <<-EOSQL
CREATE DATABASE $DB_NAME;
EOSQL
else
  echo "➤ Database \"$DB_NAME\" already exists, skipping creation."
fi

# 2) Пользователь
echo "Ensuring user \"$DB_USER\" exists…"
sudo -u postgres psql -v ON_ERROR_STOP=1 <<-EOSQL
DO
\$\$
BEGIN
   IF NOT EXISTS (SELECT FROM pg_catalog.pg_roles WHERE rolname = '$DB_USER') THEN
      CREATE USER $DB_USER WITH ENCRYPTED PASSWORD '$DB_PASS';
   END IF;
END
\$\$;
GRANT ALL PRIVILEGES ON DATABASE $DB_NAME TO $DB_USER;
EOSQL

# 3) Таблица
echo "Ensuring table tasks exists…"
sudo -u postgres psql -d "$DB_NAME" -v ON_ERROR_STOP=1 <<-'EOSQL'
CREATE TABLE IF NOT EXISTS tasks (
  id SERIAL PRIMARY KEY,
  description TEXT NOT NULL,
  deadline TIMESTAMP WITH TIME ZONE NOT NULL,
  chat_id BIGINT NOT NULL,
  notified BOOLEAN NOT NULL DEFAULT FALSE
);
EOSQL

# 4) Права на таблицу и sequence
echo "Granting permissions to $DB_USER on tasks…"
sudo -u postgres psql -d "$DB_NAME" -v ON_ERROR_STOP=1 <<-'EOSQL'
GRANT SELECT, INSERT, UPDATE ON TABLE tasks TO tasks_user;
GRANT USAGE, SELECT ON SEQUENCE tasks_id_seq TO tasks_user;
EOSQL

echo "✅ DB successfully created and configured."

# Конец скрипта (важно: пустая строка ниже)
