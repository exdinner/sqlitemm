#include "sqlitemm/stmt.hpp"

#include <cstdio>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "sqlite3.h"

#include "sqlitemm/db.hpp"
#include "sqlitemm/value.hpp"

namespace sqlitemm {

Value sqlite_value(sqlite3_stmt* stmt, const int& column) {
  switch (sqlite3_column_type(stmt, column)) {
    case SQLITE_INTEGER: return Value{Value::Integer{sqlite3_column_int64(stmt, column)}}; break;
    case SQLITE_FLOAT: return Value{Value::Float{sqlite3_column_double(stmt, column)}}; break;
    case SQLITE_TEXT: {
      const char* text_ptr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, column));
      int text_len = sqlite3_column_bytes(stmt, column);
      return Value{
        Value::Text{text_ptr, text_ptr + text_len}
      };
      break;
    }
    case SQLITE_BLOB: {
      const unsigned char* blob_ptr = reinterpret_cast<const unsigned char*>(sqlite3_column_blob(stmt, column));
      int blob_len = sqlite3_column_bytes(stmt, column);
      return Value{
        Value::Blob{blob_ptr, blob_ptr + blob_len}
      };
      break;
    }
    case SQLITE_NULL: return Value{Value::Null{nullptr}}; break;
  }
  // should not happen
  std::ignore = std::fprintf(stderr, "failed to get value from sqlite3_column, which should not happen.\n");
  return Value{Value::Null{nullptr}};
}

Stmt::~Stmt() {
  close();
}

Stmt& Stmt::bind(const int& index, const Value& value, const bool& copy) {
  sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(sqlite3_stmt_ptr);
  if (stmt == nullptr) {
    // already closed
    return *this;
  }
  const int index_max = sqlite3_bind_parameter_count(stmt);
  if (index > index_max) {
    std::ignore
      = std::fprintf(stderr, "failed to bind parameter: %d exceeds the maximum value of ID (%d).\n", index, index_max);
    return *this;
  }
  switch (value.type()) {
    case Value::Type::INTEGER: sqlite3_bind_int64(stmt, index, value.as<Value::Integer>()); break;
    case Value::Type::FLOAT: sqlite3_bind_double(stmt, index, value.as<Value::Float>()); break;
    case Value::Type::TEXT: {
      const Value::Text& text = value.as<Value::Text>();
      sqlite3_bind_text64(
        stmt, index, text.c_str(), text.length(), copy ? SQLITE_TRANSIENT : SQLITE_STATIC, SQLITE_UTF8);
      break;
    }
    case Value::Type::BLOB: {
      const Value::Blob& blob = value.as<Value::Blob>();
      sqlite3_bind_blob64(stmt, index, blob.data(), blob.size(), copy ? SQLITE_TRANSIENT : SQLITE_STATIC);
      break;
    }
    case Value::Type::NUL: sqlite3_bind_null(stmt, index); break;
    default:
      // should not happen
      std::ignore = std::fprintf(stderr, "failed to bind parameter, which should not happen.\n");
      break;
  }
  return *this;
}

Stmt& Stmt::bind(const std::string& id, const Value& value, const bool& copy) {
  const int index = sqlite3_bind_parameter_index(reinterpret_cast<sqlite3_stmt*>(sqlite3_stmt_ptr), id.c_str());
  if (index == 0) {
    std::ignore
      = std::fprintf(stderr, "failed to bind parameter: could not find SQL parameter named %s.\n", id.c_str());
    return *this;
  }
  return bind(index, value, copy);
}

Stmt& Stmt::reset() {
  int ret = sqlite3_reset(reinterpret_cast<sqlite3_stmt*>(sqlite3_stmt_ptr));
  if (ret != SQLITE_OK) {
    std::ignore = std::fprintf(
      stderr, "error while reseting Stmt: %s\n", sqlite3_errmsg(reinterpret_cast<sqlite3*>(db_ptr->sqlite3_ptr)));
  }
  return *this;
}

void Stmt::close() {
  int ret = sqlite3_finalize(reinterpret_cast<sqlite3_stmt*>(sqlite3_stmt_ptr));
  if (ret != SQLITE_OK) {
    std::ignore = std::fprintf(stderr, "failed to finalize statement (may cause memory leak): %s", sqlite3_errstr(ret));
  }
  sqlite3_stmt_ptr = nullptr;
  // TODO(rayalto): thread safety
  db_ptr->stmt_ptrs.erase(this);
}

Stmt& Stmt::each_row(const std::function<void(const std::vector<Value>&)>& callback) {
  if (sqlite3_stmt_ptr == nullptr) {
    // already closed
    return *this;
  }
  sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(sqlite3_stmt_ptr);
  int ret = sqlite3_step(stmt);
  int column_count = sqlite3_column_count(stmt);
  std::vector<Value> row(column_count, Value::of_null(nullptr));
  while (ret != SQLITE_DONE) {
    if (ret == SQLITE_ROW) {
      for (int i = 0; i < column_count; i++) {
        row[i] = std::move(sqlite_value(stmt, i));
      }
      callback(row);
      ret = sqlite3_step(stmt);
    } else {
      std::ignore = std::fprintf(stderr,
                                 "failed to setp sqlite3 statement: %s",
                                 sqlite3_errmsg(reinterpret_cast<sqlite3*>(db_ptr->sqlite3_ptr)));
      break;
    }
  }
  return *this;
}

Stmt& Stmt::each_row(const std::function<void(const std::vector<std::string>&, const std::vector<Value>&)>& callback) {
  const std::vector<std::string> column_names = this->column_names();
  return each_row([&column_names, &callback](const std::vector<Value>& row) -> void {
    callback(column_names, row);
  });
}

std::vector<std::string> Stmt::column_names() {
  sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(sqlite3_stmt_ptr);
  int column_count = sqlite3_column_count(stmt);
  std::vector<std::string> names;
  names.reserve(column_count);
  for (int i = 0; i < column_count; i++) {
    names.emplace_back(sqlite3_column_name(stmt, i));
  }
  return names;
}

std::vector<std::vector<Value>> Stmt::all_rows() {
  if (sqlite3_stmt_ptr == nullptr) {
    // already closed
    return {};
  }
  sqlite3_stmt* stmt = reinterpret_cast<sqlite3_stmt*>(sqlite3_stmt_ptr);
  int ret = sqlite3_step(stmt);
  int column_count = sqlite3_column_count(stmt);
  std::vector<std::vector<Value>> all;
  while (ret != SQLITE_DONE) {
    std::vector<Value> row;
    row.reserve(column_count);
    if (ret == SQLITE_ROW) {
      for (int i = 0; i < column_count; i++) {
        row.emplace_back(std::move(sqlite_value(stmt, i)));
      }
      all.emplace_back(std::move(row));
      ret = sqlite3_step(stmt);
    } else {
      std::ignore = std::fprintf(stderr,
                                 "failed to setp sqlite3 statement: %s",
                                 sqlite3_errmsg(reinterpret_cast<sqlite3*>(db_ptr->sqlite3_ptr)));
      break;
    }
  }
  return all;
}

Stmt::Stmt(DB* db, const std::string& statement) : db_ptr(db) {
  sqlite3_prepare_v2(reinterpret_cast<sqlite3*>(db_ptr->sqlite3_ptr),
                     statement.c_str(),
                     static_cast<int>(statement.length()),
                     reinterpret_cast<sqlite3_stmt**>(&sqlite3_stmt_ptr),
                     nullptr);
}

} // namespace sqlitemm
