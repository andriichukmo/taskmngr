#include "worker.h"
#include "Database.h"
#include "TelegramBot.h"
#include "config.h"
#include <chrono>
#include <iostream>
#include <thread>

namespace taskmngrbot {

Worker::Worker(Database &db, TelegramBot &bot) : db_(db), bot_(bot) {}

void Worker::run() {
  while (true) {
    try {
      auto tasks = db_.GetDueTasks();
      for (auto &t : tasks) {
        std::string msg = "Напоминание! Задача просрочена:\n" + t.description;
        bot_.SendMessage(t.chat_id, msg);
        db_.MarkNotified(t.id);
      }
    } catch (const std::exception &e) {
      std::cerr << "Worker error : " << e.what() << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(Config::POLL_INTERVAL));
  }
}

} // namespace taskmngrbot
