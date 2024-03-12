cmake_minimum_required(VERSION 3.8)
project(
  sqlitemm
  LANGUAGES CXX
  VERSION 1.0.0
  DESCRIPTION "C++ binding of sqlite3"
)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE REQUIRED sqlite3)

set(${PROJECT_NAME}_INCLUDES
  ${PROJECT_SOURCE_DIR}/include
)

set(${PROJECT_NAME}_SRCS
  ${PROJECT_SOURCE_DIR}/src/db.cpp
  ${PROJECT_SOURCE_DIR}/src/stmt.cpp
  ${PROJECT_SOURCE_DIR}/src/value.cpp
)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${${PROJECT_NAME}_INCLUDES}>)
target_link_libraries(${PROJECT_NAME} PRIVATE ${SQLITE_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${SQLITE_INCLUDE_DIRS})

add_subdirectory(test)

install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}-targets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ${PROJECT_NAME}-targets
  FILE ${PROJECT_NAME}-targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/config.cmake.in
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
  ${PROJECT_NAME}-config-version.cmake
  VERSION ${PACKAGE_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
