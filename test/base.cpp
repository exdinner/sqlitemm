#include <cstdio>

#include "sqlitemm/db.hpp"

/* NOLINTNEXTLINE(misc-unused-parameters) */
int main(int argc, const char* argv[]) {
  std::printf("sqlite version: %s\n", sqlitemm::sqlite_version());
  return 0;
}
