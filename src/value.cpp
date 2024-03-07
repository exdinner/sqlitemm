#include "sqlitemm/value.hpp"

#include <utility>

namespace sqlitemm {

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
