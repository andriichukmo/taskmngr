#pragma once
#include <string>

namespace Config {
static const std::string DB_CONN =
    "host=localhost port=5432 dbname=tasks user=tasks_user password=tasks_pass";
static const std::string BOT_TOKEN = "...";
static const int POLL_INTERVAL = 5;
static const std::string ADMIN_PASSWORD = "...";
static const std::string LISTENER_TOKEN = "...";
} // namespace Config
