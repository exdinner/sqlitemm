#ifndef SQLITEMM_SQLITEMM_DB_HPP_
#define SQLITEMM_SQLITEMM_DB_HPP_

#include <filesystem>
#include <string>
#include <unordered_set>

namespace sqlitemm {

/* include/sqlitemm/stmt.hpp */
class Stmt;

class DB {
  friend class Stmt;

public:
  // create a private, temporary in-memory database
  DB();
  // open database `file`, or create a private temporary on-disk database
  // if `file.empty()`
  explicit DB(const std::filesystem::path& file);
  DB(DB&& db_old) noexcept;
  DB& operator=(DB&& db_old) noexcept;
  DB(const DB&) = delete;
  DB& operator=(const DB&) = delete;

  virtual ~DB();

  void close();

  Stmt prepare(const std::string& statement);

protected:
  void* sqlite3_ptr{nullptr};
  std::unordered_set<Stmt*> stmt_ptrs;
  std::filesystem::path db_file;

  void open();
};

const char* sqlite_version();
// if sqlite3 is compiled to be thread safe
bool sqlite_thread_safe();

} // namespace sqlitemm

#endif // SQLITEMM_SQLITEMM_DB_HPP_
