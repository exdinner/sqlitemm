#ifndef SQLITEMM_SQLITEMM_VALUE_HPP_
#define SQLITEMM_SQLITEMM_VALUE_HPP_

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace sqlitemm {

class Value {
public:
  using Integer = std::int64_t;
  using Float = double;
  using Text = std::string;
  using Blob = std::vector<std::uint8_t>;
  using Null = std::nullptr_t;

  enum class Type : std::uint8_t { INTEGER, FLOAT, TEXT, BLOB, NUL };

  static Value of_integer(const Integer& i);
  static Value of_float(const Float& f);
  static Value of_text(const Text& t);
  static Value of_text(Text&& t);
  static Value of_blob(const Blob& b);
  static Value of_blob(Blob&& b);
  static Value of_null(const Null& n);

  Value();
  explicit Value(const Integer& i);
  explicit Value(const Float& f);
  explicit Value(const Text& t);
  explicit Value(Text&& t);
  explicit Value(const Blob& b);
  explicit Value(Blob&& b);
  explicit Value(const Null& n);
  Value(const Value&) = default;
  Value(Value&&) noexcept = default;
  Value& operator=(const Value&) = default;
  Value& operator=(Value&&) noexcept = default;

  virtual ~Value() = default;

  [[nodiscard]] const Type& type() const&;

  template <typename T>
  const T& as() const& {
    return std::get<T>(v);
  }

  template <typename T>
  void reset(const T& value) {
    *this = Value{value};
  }

  template <typename T>
  void reset(T&& value) {
    *this = std::move(Value{std::forward<T>(value)});
  }

protected:
  std::variant<Integer, Float, Text, Blob, Null> v;
  Type t;
};

} // namespace sqlitemm

#endif // SQLITEMM_SQLITEMM_VALUE_HPP_
