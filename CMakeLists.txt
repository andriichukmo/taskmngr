cmake_minimum_required(VERSION 3.22)
project(taskmngr)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(nlohmann_json REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PQXX_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
)

add_executable(taskmgr
    src/main.cpp
    src/Database.cpp
    src/taskmanager.cpp
    src/except.cpp
    src/TelegramBot.cpp
    src/worker.cpp
    src/AdminBot.cpp
)

target_link_libraries(taskmgr
    ${PQXX_LIBRARIES}
    ${CURL_LIBRARIES}
    Threads::Threads
    nlohmann_json::nlohmann_json
)

target_include_directories(taskmgr
    PRIVATE
      ${PROJECT_SOURCE_DIR}/include
      ${PQXX_INCLUDE_DIRS}
      ${CURL_INCLUDE_DIRS}
)
