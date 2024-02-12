#include "sqlitemm/stmt.hpp"

#include "sqlite3.h"

#include "sqlitemm/db.hpp"

namespace sqlitemm {

void Stmt::close() {
  sqlite3_finalize(reinterpret_cast<sqlite3_stmt*>(sqlite3_stmt_ptr));
  db_ptr->remove_stmt(this);
}

} // namespace sqlitemm
