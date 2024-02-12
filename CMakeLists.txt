cmake_minimum_required(VERSION 3.8)
project(
  sqlitemm
  LANGUAGES CXX
  VERSION 1.0.0
  DESCRIPTION "C++ binding of sqlite3")
option(BUILD_TESTS "Build sources in `/test` directory" ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE REQUIRED sqlite3)

set(${CMAKE_PROJECT_NAME}_INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(${CMAKE_PROJECT_NAME}_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/db.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/stmt.cpp)

add_library(${CMAKE_PROJECT_NAME} STATIC ${${CMAKE_PROJECT_NAME}_SRCS})
target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${${CMAKE_PROJECT_NAME}_INCLUDES})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${SQLITE_LIBRARIES})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${SQLITE_INCLUDE_DIRS})

if(BUILD_TESTS)
  add_subdirectory(test)
endif()
