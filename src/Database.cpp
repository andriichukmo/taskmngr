#include "Database.h"
#include <exception>
#include <iostream>
#include <stdexcept>

namespace taskmngrbot {

Database::Database(const std::string &connstr) : conn_(connstr) {
  if (!conn_.is_open()) {
    throw std::runtime_error("Cannot open Database connection");
  }
}

void Database::AddTask(const std::string &desc, std::time_t deadline,
                       int64_t chat_id) {
  try {
    pqxx::work txn(conn_);
    txn.exec_params("INSERT INTO tasks (description, deadline, chat_id) "
                    "VALUES ($1, to_timestamp($2), $3);",
                    desc, static_cast<long>(deadline), chat_id);
    txn.commit();
  } catch (const std::exception &e) {
    std::cerr << "Database::AddTask error: " << e.what() << std::endl;
    throw;
  }
}

std::vector<Task> Database::GetDueTasks() {
  std::vector<Task> tasks;
  try {
    pqxx::work txn(conn_);
    long now = static_cast<long>(std::time(nullptr));
    pqxx::result res = txn.exec_params(
        "SELECT id, description, extract(epoch FROM deadline)::bigint, chat_id "
        "FROM tasks "
        "WHERE deadline <= to_timestamp($1) AND notified = false;",
        now);
    for (auto const &row : res) {
      tasks.push_back(Task{row[0].as<int>(), row[1].as<std::string>(),
                           static_cast<std::time_t>(row[2].as<long>()),
                           row[3].as<long long>()});
    }
  } catch (const std::exception &e) {
    std::cerr << "Database::GetDueTask error: " << e.what() << std::endl;
    throw;
  }
  return tasks;
}

void Database::MarkNotified(int id) {
  try {
    pqxx::work txn(conn_);
    txn.exec_params("UPDATE tasks SET notified = TRUE WHERE id = $1;", id);
    txn.commit();
  } catch (const std::exception &e) {
    std::cerr << "Database::MarkNotifier error: " << e.what() << std::endl;
    throw;
  }
}

} // namespace taskmngrbot
