#pragma once
#include "Database.h"
#include "TelegramBot.h"

namespace taskmngrbot {

class Worker {
public:
  Worker(Database &db, TelegramBot &bot);
  void run();

private:
  Database &db_;
  TelegramBot &bot_;
};

} // namespace taskmngrbot
