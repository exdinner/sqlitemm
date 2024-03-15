#ifndef SQLITEMM_SQLITEMM_STMT_HPP_
#define SQLITEMM_SQLITEMM_STMT_HPP_

#include <cstdint>
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
  // reset `Stmt` back to its initial state, ready to be re-executed
  // bound `Value` s are retained, use `clear_bindings` to reset the bindings
  // or use `bind` to overwrite the previous binding
  Stmt& reset();
  Stmt& clear_bindings();
  void close();
  Stmt& each_row(const std::function<void(const std::vector<std::string>&, const std::vector<Value>&)>& callback);
  Stmt& each_row(const std::function<void(const std::vector<Value>&)>& callback);
  Stmt& each_row();
  [[nodiscard]] std::int64_t changes();
  [[nodiscard]] std::string column_name(int column_index);
  [[nodiscard]] std::vector<std::string> column_names();
  [[nodiscard]] std::vector<std::vector<Value>> all_rows();

protected:
  explicit Stmt(DB* db, const std::string& statement);

  void* sqlite3_stmt_ptr{nullptr};
  DB* db_ptr{nullptr};
};

} // namespace sqlitemm

#endif // SQLITEMM_SQLITEMM_STMT_HPP_
