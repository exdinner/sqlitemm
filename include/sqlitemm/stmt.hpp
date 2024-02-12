#ifndef SQLITEMM_SQLITEMM_STMT_HPP_
#define SQLITEMM_SQLITEMM_STMT_HPP_

namespace sqlitemm {

/* include/sqlitemm/db.hpp */
class DB;

class Stmt {
  friend class DB;

public:
  Stmt() = delete;
  Stmt(Stmt&&) noexcept;
  Stmt& operator=(Stmt&&) noexcept;
  Stmt(const Stmt&) = delete;
  Stmt& operator=(const Stmt&) = delete;

  virtual ~Stmt() = default;

  void close();

protected:
  void* sqlite3_stmt_ptr{nullptr};
  DB* db_ptr{nullptr};
};

} // namespace sqlitemm

#endif // SQLITEMM_SQLITEMM_STMT_HPP_
