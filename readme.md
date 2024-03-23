# SQLitemm

A shitty C++ binding of [sqlite](https://sqlite.org).

## Usage

```cpp
#include "sqlitemm/db.hpp"

// ...

sqlitemm::DB{"/home/rayalto/Documents/test.db"}
  .prepare("select * from customers where cust_id == ?;")
  .bind(1, sqlitemm::Value::of_integer(10001))
  .each_row([](const std::vector<sqlitemm::Value>& row) -> void {
    std::printf("name: %s\n", row[1].as<sqlitemm::Value::Text>().c_str());
  });
```

## Build

```sh
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=OFF -S . -B ./build
cmake --build ./build
cmake --install ./build
```

## Linking

```cmake
find_package(sqlitemm REQUIRED CONFIG)

# ...

target_link_libraries(your_target PRIVATE sqlitemm::sqlitemm)
```
