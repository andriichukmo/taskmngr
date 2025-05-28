#include "AdminBot.h"
#include "config.h"

#include <chrono>
#include <condition_variable>
#include <curl/curl.h>
#include <curl/easy.h>
#include <exception>
#include <iostream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <thread>

namespace taskmngrbot {

using json = nlohmann::json;

static size_t writeCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

AdminBot::AdminBot(const std::string &token, const std::string &password,
                   Database &db)
    : token_(token), password_(password), db_(db) {
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

static void curlCleanup() { curl_global_cleanup(); }

void AdminBot::run() {
  std::queue<json> q;
  std::mutex mtx;
  std::condition_variable cv;
  bool stop = false;

  std::thread poller([&]() {
    while (!stop) {
      try {
        std::ostringstream param;
        param << "offset=" << (last_update_id_ + 1) << "&timeout=0";
        std::string resp = apiGet("getUpdates", param.str());
        auto j = json::parse(resp);
        {
          std::lock_guard<std::mutex> lk(mtx);
          for (auto &upd : j["result"]) {
            q.push(upd);
          }
        }
        cv.notify_one();
      } catch (std::exception &e) {
        std::cerr << "Poller error: " << e.what() << std::endl;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });
  while (true) {
    json upd;
    {
      std::unique_lock<std::mutex> lk(mtx);
      cv.wait(lk, [&] { return !q.empty(); });
      upd = q.front();
      q.pop();
    }
    last_update_id_ = upd["update_id"].get<int>();
    if (upd.contains("message")) {
      auto &msg = upd["message"];
      int64_t chat = msg["chat"]["id"].get<int64_t>();
      std::string text = msg["text"].get<std::string>();
      processMessage(chat, text);
    }
  }
  stop = true;
  poller.join();
}

void AdminBot::processMessage(int64_t chat_id, const std::string &text) {
  std::istringstream ss(text);
  std::string cmd, pwd, date, time;
  ss >> cmd >> pwd >> date >> time;
  if (cmd != "/add" || pwd != password_) {
    sendMessage(chat_id,
                "Для того чтобы добавить дедлайн, необходимо написать его в "
                "формате ```/add <пароль> YYYY-MM-DD HH:MM Описание задачи```");
    return;
  }
  std::string desc;
  std::getline(ss, desc);
  if (desc.size() > 0)
    desc.erase(0, 1);
  std::tm tm = {};
  std::istringstream ds(date + " " + time);
  ds >> std::get_time(&tm, "%Y-%m-%d %H:%M");
  if (ds.fail()) {
    sendMessage(chat_id, "Неверный формат даты/времени");
    return;
  }
  std::time_t deadline = std::mktime(&tm);
  try {
    db_.AddTask(desc, deadline, chat_id);
    sendMessage(chat_id, "Задача добавлена\n" + desc + "\nс дедлайном\n" +
                             date + " " + time);
  } catch (std::exception &e) {
    sendMessage(chat_id, std::string("Ошибка БД: ") + e.what());
  }
}

void AdminBot::sendMessage(int64_t chat_id, const std::string &text) {
  std::ostringstream data;
  data << "chat_id=" << chat_id
       << "&text=" << curl_easy_escape(nullptr, text.c_str(), text.size());
  apiPost("sendMessage", data.str());
}

std::string AdminBot::apiGet(const std::string &method,
                             const std::string &params) {
  CURL *curl = curl_easy_init();
  std::string readBuf;
  std::ostringstream url;
  url << "https://api.telegram.org/bot" << token_ << "/" << method << "?"
      << params;
  curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuf);
  curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  return readBuf;
}

std::string AdminBot::apiPost(const std::string &method,
                              const std::string &postfield) {
  CURL *curl = curl_easy_init();
  std::string readBuf;
  std::ostringstream url;
  url << "https://api.telegram.org/bot" << token_ << "/" << method;
  curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfield.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuf);
  CURLcode res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  if (res != CURLE_OK) {
    throw std::runtime_error("CURL POST FAILED");
  }
  return readBuf;
}

} // namespace taskmngrbot
