#pragma once

#include <cstdint>
#include <ctime>
#include <string>

namespace taskmngrbot {

struct Task {
  int id;
  std::string description;
  std::time_t deadline;
  int64_t chat_id;
};

} // namespace taskmngrbot
