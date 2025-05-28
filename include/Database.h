#pragma once

#include "task.h"
#include <pqxx/pqxx>
#include <vector>

namespace taskmngrbot {

class Database {
public:
  explicit Database(const std::string &connstr);
  void AddTask(const std::string &description, std::time_t deadline,
               int64_t chat_id);
  std::vector<Task> GetDueTasks();
  void MarkNotified(int id);

private:
  pqxx::connection conn_;
};

} // namespace taskmngrbot
