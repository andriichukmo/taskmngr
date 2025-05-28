#include "except.h"
#include "taskmanager.h"

int main() {
  try {
    tskmngr::TaskManager mgr;
    mgr.run();
  } catch (...) {
    except::react();
  }
  return 0;
}
