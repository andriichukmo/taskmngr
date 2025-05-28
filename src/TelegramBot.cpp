#include "TelegramBot.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <iostream>
#include <sstream>

namespace taskmngrbot {

TelegramBot::TelegramBot(const std::string &token) : token_(token) {
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

TelegramBot::~TelegramBot() { curl_global_cleanup(); }

void TelegramBot::SendMessage(int64_t chat_id, const std::string &text) {
  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "curl init failed" << std::endl;
    return;
  }

  std::ostringstream url;
  url << "https://api.telegram.org/bot" << token_ << "/sendMessage"
      << "?chat_id=" << chat_id
      << "&text=" << curl_easy_escape(curl, text.c_str(), text.size());
  curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    std::cerr << "Telegram send failed : " << curl_easy_strerror(res)
              << std::endl;
  }
  curl_easy_cleanup(curl);
}

} // namespace taskmngrbot
