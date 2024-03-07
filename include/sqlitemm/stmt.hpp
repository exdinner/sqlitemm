#ifndef SQLITEMM_SQLITEMM_STMT_HPP_
#define SQLITEMM_SQLITEMM_STMT_HPP_

#include <functional>
#include <string>
#include <vector>

#include "sqlitemm/value.hpp"

namespace sqlitemm {

/* include/sqlitemm/db.hpp */
class DB;

class Stmt {
public:
  friend class DB;

  Stmt() = delete;
  Stmt(const Stmt&) = delete;
  Stmt(Stmt&&) noexcept;
  Stmt& operator=(const Stmt&) = delete;
  Stmt& operator=(Stmt&&) noexcept;

  virtual ~Stmt();

  Stmt& bind(const int& index, const Value& value, const bool& copy = true);
  Stmt& bind(const std::string& id, const Value& value, const bool& copy = true);
  void close();
  Stmt& each_row(const std::function<void(const std::vector<Value>&)>& callback);
  std::vector<std::string> column_names();
  std::vector<std::vector<Value>> all_rows();

protected:
  explicit Stmt(DB* db, const std::string& statement);

  void* sqlite3_stmt_ptr{nullptr};
  DB* db_ptr{nullptr};
};

} // namespace sqlitemm

#endif // SQLITEMM_SQLITEMM_STMT_HPP_
