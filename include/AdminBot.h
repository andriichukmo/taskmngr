#pragma once
#include "Database.h"
#include <cstdint>
#include <string>

namespace taskmngrbot {

class AdminBot {
public:
  AdminBot(const std::string &token, const std::string &password, Database &db);
  void run();

private:
  std::string token_;
  std::string password_;
  Database &db_;
  int64_t last_update_id_ = 0;

  void processMessage(int64_t chat_id, const std::string &text);
  void sendMessage(int64_t chat_id, const std::string &tetx);
  std::string apiGet(const std::string &method, const std::string &params);
  std::string apiPost(const std::string &method, const std::string &postfield);
};

} // namespace taskmngrbot
