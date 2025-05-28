#include "except.h"
#include <cstdlib>
#include <exception>
#include <iostream>

namespace except {

void react() {
  try {
    throw;
  } catch (const std::exception &e) {
    std::cerr << "[FATAL] got error : " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "[FATAL] Unknown error 0_0\n";
  }
  std::exit(EXIT_FAILURE);
}

} // namespace except
