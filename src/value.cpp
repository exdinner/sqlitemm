#include "sqlitemm/value.hpp"

#include <utility>

namespace sqlitemm {

Value Value::of_integer(const Integer& i) {
  return Value{i};
}

Value Value::of_float(const Float& f) {
  return Value{f};
}

Value Value::of_text(const Text& t) {
  return Value{t};
}

Value Value::of_text(Text&& t) {
  return Value{std::move(t)};
}

Value Value::of_blob(const Blob& b) {
  return Value{b};
}

Value Value::of_blob(Blob&& b) {
  return Value{std::move(b)};
}

Value Value::of_null(const Null& n) {
  return Value{n};
}

Value::Value() : v(nullptr), t(Type::NUL) {
}

Value::Value(const Integer& i) : v(i), t(Type::INTEGER) {
}

Value::Value(const Float& f) : v(f), t(Type::FLOAT) {
}

Value::Value(const Text& t) : v(t), t(Type::TEXT) {
}

Value::Value(Text&& t) : v(std::move(t)), t(Type::TEXT) {
}

Value::Value(const Blob& b) : v(b), t(Type::BLOB) {
}

Value::Value(Blob&& b) : v(std::move(b)), t(Type::BLOB) {
}

Value::Value(const Null& n) : v(n), t(Type::NUL) {
}

[[nodiscard]] const Value::Type& Value::type() const& {
  return t;
}

} // namespace sqlitemm
