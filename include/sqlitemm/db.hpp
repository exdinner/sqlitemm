#ifndef SQLITEMM_SQLITEMM_DB_HPP_
#define SQLITEMM_SQLITEMM_DB_HPP_

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "sqlitemm/value.hpp"

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
  [[nodiscard]] Stmt prepare(std::string_view statement);
  // a wrapper around `DB::prepare` and `Stmt::each_row`
  DB& exec(std::string_view statement,
           const std::function<void(const std::vector<std::string>&, const std::vector<Value>&)>& callback);
  DB& exec(std::string_view statement, const std::function<void(const std::vector<Value>&)>& callback);
  DB& exec(std::string_view statement);
  // return the number of rows modified, inserted or deleted by the most
  // recently completed `INSERT`, `UPDATE` or `DELETE` statement on this
  // database connection(`DB`)
  [[nodiscard]] std::int64_t changes();
  [[nodiscard]] std::int64_t total_changes();

  // return names of all tables in the database
  // by exec `SELECT name from sqlite_schema where type == 'table';`
  // doc: https://www.sqlite.org/schematab.html
  [[nodiscard]] std::vector<std::string> table_names();

  // if the db is in autocommit mode
  [[nodiscard]] bool autocommit();

protected:
  void* sqlite3_ptr_{nullptr};
  std::unordered_set<Stmt*> stmt_ptrs_;
  std::filesystem::path db_file_;

  void open();
};

const char* sqlite_version();
// if sqlite3 is compiled to be thread safe
bool sqlite_thread_safe();

} // namespace sqlitemm

#endif // SQLITEMM_SQLITEMM_DB_HPP_
