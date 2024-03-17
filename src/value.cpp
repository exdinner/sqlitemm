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

Value::Value() : v_(nullptr), t_(Type::NUL) {
}

Value::Value(const Integer& i) : v_(i), t_(Type::INTEGER) {
}

Value::Value(const Float& f) : v_(f), t_(Type::FLOAT) {
}

Value::Value(const Text& t) : v_(t), t_(Type::TEXT) {
}

Value::Value(Text&& t) : v_(std::move(t)), t_(Type::TEXT) {
}

Value::Value(const Blob& b) : v_(b), t_(Type::BLOB) {
}

Value::Value(Blob&& b) : v_(std::move(b)), t_(Type::BLOB) {
}

Value::Value(const Null& n) : v_(n), t_(Type::NUL) {
}

[[nodiscard]] const Value::Type& Value::type() const& {
  return t_;
}

} // namespace sqlitemm
