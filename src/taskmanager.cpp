#include "taskmanager.h"
#include "AdminBot.h"
#include "Database.h"
#include "TelegramBot.h"
#include "config.h"
#include "worker.h"

#include <chrono>
#include <thread>

namespace tskmngr {

using namespace taskmngrbot;

TaskManager::TaskManager() {}

TaskManager::~TaskManager() {}

void TaskManager::run() {
  Database db(Config::DB_CONN);
  TelegramBot notifier(Config::BOT_TOKEN);
  AdminBot admin(Config::LISTENER_TOKEN, Config::ADMIN_PASSWORD, db);
  std::thread adminTh([&admin]() { admin.run(); });
  adminTh.detach();

  Worker worker(db, notifier);
  std::thread workerTh([&worker]() { worker.run(); });
  workerTh.detach();

  while (true) {
    std::this_thread::sleep_for(std::chrono::hours(24));
  }
}

} // namespace tskmngr
