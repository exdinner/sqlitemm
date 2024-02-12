
#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

#include "sqlite3.h"

#include "sqlitemm/db.hpp"
#include "sqlitemm/stmt.hpp"

namespace sqlitemm {

DB::DB() {
  open(":memory:");
}

DB::DB(const std::filesystem::path& file) {
  open(file.string());
}

DB::DB(DB&& db_old) noexcept
  : sqlite3_ptr(db_old.sqlite3_ptr)
  , stmt_ptrs(std::move(db_old.stmt_ptrs))
  , db_file(std::move(db_old.db_file)) {
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

void DB::open(const std::string_view& filename) {
  // close the previous connection
  if (sqlite3_ptr != nullptr) {
    close();
  }

  int ret = sqlite3_open(filename.data(), reinterpret_cast<sqlite3**>(&sqlite3_ptr));
  if (ret != SQLITE_OK) {
    std::cerr << "failed to open database `" << filename
              << "`: " << sqlite3_errmsg(reinterpret_cast<sqlite3*>(sqlite3_ptr)) << '\n';
    sqlite3_close(reinterpret_cast<sqlite3*>(sqlite3_ptr));
    sqlite3_ptr = nullptr;
    return;
  }
}

void DB::close() {
  // finalize all sqlite3_stmt
  for (Stmt* stmt : stmt_ptrs) {
    stmt->close();
  }
  // close connection
  int ret = sqlite3_close(reinterpret_cast<sqlite3*>(sqlite3_ptr));
  if (ret != SQLITE_OK) {
    std::cerr << "failed to close database (may cause memory leak): "
              << sqlite3_errmsg(reinterpret_cast<sqlite3*>(sqlite3_ptr)) << '\n';
  }
  sqlite3_ptr = nullptr;
}

void DB::remove_stmt(Stmt* stmt) {
  stmt_ptrs.erase(stmt);
}

const char* sqlite_version() {
  return sqlite3_libversion();
}

} // namespace sqlitemm
