#include <cstdio>
#include <string>
#include <tuple>
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
  sqlitemm::DB db{"test.db"};
  sqlitemm::Stmt stmt = db.prepare("select * from customers;");
  for (const std::string& column_name : stmt.column_names()) {
    std::printf("%s\n", column_name.c_str());
  }
  std::printf("\n");
  stmt.each_row([&](const std::vector<sqlitemm::Value>& row) -> void {
    std::printf("%s\n", row[1].as<sqlitemm::Value::Text>().c_str());
  });
  return 0;
}
