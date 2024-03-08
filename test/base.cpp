#include <cstdio>
#include <string>
#include <vector>

#include "sqlitemm/db.hpp"
#include "sqlitemm/stmt.hpp"
#include "sqlitemm/value.hpp"

/* NOLINTNEXTLINE(misc-unused-parameters) */
int main(int argc, const char* argv[]) {
  std::printf("sqlite version: %s\nthread safety: %s\n",
              sqlitemm::sqlite_version(),
              sqlitemm::sqlite_thread_safe() ? "fine" : "fuck");
  std::printf("\n");
  sqlitemm::DB{"./build/test/test.db"}.exec(
    "select * from customers;",
    [](const std::vector<std::string>& column_names, const std::vector<sqlitemm::Value>& row) -> void {
    std::printf("%s: %s\n", column_names[1].c_str(), row[1].as<sqlitemm::Value::Text>().c_str());
  });
  return 0;
}
