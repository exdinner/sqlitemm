#include "sqlitemm/db.hpp"

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <string>
#include <utility>

#include "sqlite3.h"

#include "sqlitemm/stmt.hpp"

namespace sqlitemm {

DB::DB() : db_file(":memory:") {
  open();
}

DB::DB(const std::filesystem::path& file) : db_file(file) {
  open();
}

DB::DB(DB&& db_old) noexcept
  : sqlite3_ptr{db_old.sqlite3_ptr}
  , stmt_ptrs{std::move(db_old.stmt_ptrs)}
  , db_file{std::move(db_old.db_file)} {
  db_old.sqlite3_ptr = nullptr;
}

DB& DB::operator=(DB&& db_old) noexcept {
  sqlite3_ptr = db_old.sqlite3_ptr;
  db_old.sqlite3_ptr = nullptr;
  stmt_ptrs = std::move(db_old.stmt_ptrs);
  db_file = std::move(db_old.db_file);
  return *this;
}

DB::~DB() {
  close();
}

void DB::close() {
  // finalize all sqlite3_stmt
  for (Stmt* stmt : stmt_ptrs) {
    stmt->close();
  }
  // close connection
  int ret = sqlite3_close(reinterpret_cast<sqlite3*>(sqlite3_ptr));
  if (ret != SQLITE_OK) {
    std::ignore = std::fprintf(stderr,
                               "failed to close database (may cause memory leak): %s\n",
                               sqlite3_errmsg(reinterpret_cast<sqlite3*>(sqlite3_ptr)));
  }
  sqlite3_ptr = nullptr;
}

[[nodiscard]] Stmt DB::prepare(const std::string& statement) {
  return Stmt(this, statement);
}

DB& DB::exec(const std::string& statement,
             const std::function<void(const std::vector<std::string>&, const std::vector<Value>&)>& callback) {
  Stmt stmt{prepare(statement)};
  stmt.each_row(callback);
  return *this;
}

DB& DB::exec(const std::string& statement, const std::function<void(const std::vector<Value>&)>& callback) {
  Stmt stmt{prepare(statement)};
  stmt.each_row(callback);
  return *this;
}

[[nodiscard]] std::int64_t DB::changes() {
  return sqlite3_changes64(reinterpret_cast<sqlite3*>(sqlite3_ptr));
}

[[nodiscard]] std::int64_t DB::total_changes() {
  return sqlite3_total_changes64(reinterpret_cast<sqlite3*>(sqlite3_ptr));
}

void DB::open() {
  // close the previous connection
  if (sqlite3_ptr != nullptr) {
    close();
  }

  int ret = sqlite3_open(db_file.c_str(), reinterpret_cast<sqlite3**>(&sqlite3_ptr));
  if (ret != SQLITE_OK) {
    std::ignore = std::fprintf(stderr,
                               "failed to open database `%s`: %s\n",
                               db_file.c_str(),
                               sqlite3_errmsg(reinterpret_cast<sqlite3*>(sqlite3_ptr)));
    sqlite3_close(reinterpret_cast<sqlite3*>(sqlite3_ptr));
    sqlite3_ptr = nullptr;
    return;
  }
}

const char* sqlite_version() {
  return sqlite3_libversion();
}

bool sqlite_thread_safe() {
  return sqlite3_threadsafe() != 0;
}

} // namespace sqlitemm
