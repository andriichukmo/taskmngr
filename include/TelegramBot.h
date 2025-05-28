#pragma once
#include <cstdint>
#include <string>

namespace taskmngrbot {

class TelegramBot {
public:
  explicit TelegramBot(const std::string &token);
  ~TelegramBot();
  void SendMessage(int64_t chat_id, const std::string &text);

private:
  std::string token_;
};

} // namespace taskmngrbot
