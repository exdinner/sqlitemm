#include "sqlitemm/db.hpp"

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

#include "sqlite3.h"

#include "sqlitemm/stmt.hpp"
#include "sqlitemm/value.hpp"

namespace sqlitemm {

DB::DB() : db_file_(":memory:") {
  open();
}

DB::DB(const std::filesystem::path& file) : db_file_(file) {
  open();
}

DB::DB(DB&& db_old) noexcept
  : sqlite3_ptr_{db_old.sqlite3_ptr_}
  , stmt_ptrs_{std::move(db_old.stmt_ptrs_)}
  , db_file_{std::move(db_old.db_file_)} {
  db_old.sqlite3_ptr_ = nullptr;
}

DB& DB::operator=(DB&& db_old) noexcept {
  sqlite3_ptr_ = db_old.sqlite3_ptr_;
  db_old.sqlite3_ptr_ = nullptr;
  stmt_ptrs_ = std::move(db_old.stmt_ptrs_);
  db_file_ = std::move(db_old.db_file_);
  return *this;
}

DB::~DB() {
  close();
}

void DB::close() {
  // finalize all sqlite3_stmt
  for (Stmt* stmt : stmt_ptrs_) {
    stmt->close();
  }
  // close connection
  int ret = sqlite3_close(reinterpret_cast<sqlite3*>(sqlite3_ptr_));
  if (ret != SQLITE_OK) {
    std::ignore = std::fprintf(stderr,
                               "failed to close database (may cause memory leak): %s\n",
                               sqlite3_errmsg(reinterpret_cast<sqlite3*>(sqlite3_ptr_)));
  }
  sqlite3_ptr_ = nullptr;
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

DB& DB::exec(const std::string& statement) {
  Stmt stmt{prepare(statement)};
  stmt.each_row();
  return *this;
}

[[nodiscard]] std::int64_t DB::changes() {
  return sqlite3_changes64(reinterpret_cast<sqlite3*>(sqlite3_ptr_));
}

[[nodiscard]] std::vector<std::string> DB::table_names() {
  std::vector<std::string> names;
  exec("SELECT name from sqlite_schema where type == 'table';", [&names](const std::vector<Value>& row) -> void {
    for (const Value& name : row) {
      names.emplace_back(name.as<Value::Text>());
    }
  });
  return names;
}

[[nodiscard]] bool DB::autocommit() {
  return sqlite3_get_autocommit(reinterpret_cast<sqlite3*>(sqlite3_ptr_)) != 0;
}

[[nodiscard]] std::int64_t DB::total_changes() {
  return sqlite3_total_changes64(reinterpret_cast<sqlite3*>(sqlite3_ptr_));
}

void DB::open() {
  // close the previous connection
  if (sqlite3_ptr_ != nullptr) {
    close();
  }

  int ret = sqlite3_open(db_file_.c_str(), reinterpret_cast<sqlite3**>(&sqlite3_ptr_));
  if (ret != SQLITE_OK) {
    std::ignore = std::fprintf(stderr,
                               "failed to open database `%s`: %s\n",
                               db_file_.c_str(),
                               sqlite3_errmsg(reinterpret_cast<sqlite3*>(sqlite3_ptr_)));
    sqlite3_close(reinterpret_cast<sqlite3*>(sqlite3_ptr_));
    sqlite3_ptr_ = nullptr;
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
