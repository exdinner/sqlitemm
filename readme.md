# SQLitemm

A shitty C++ binding of [sqlite](https://sqlite.org).

## Usage

```cpp
#include "sqlitemm/db.hpp"

// ...

sqlitemm::DB{"some.random.database.db"}.exec(
  "select * from customers;",

  [](const std::vector<std::string>& column_names,
     const std::vector<sqlitemm::Value>& row) -> void {

    std::printf("%s: %s\n",
      column_names[1].c_str(),
      row[1].as<sqlitemm::Value::Text>().c_str()
    );

  }
);
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
