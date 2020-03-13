#-------------------------------
cmake_minimum_required(VERSION 3.8)
project(SqlWrapper VERSION 1.0.0 LANGUAGES CXX)

option(SQLITE_BACKEND "build with sqlite support" ON)
option(POSTGRESQL_BACKEND "build with PostgreSql support" ON)

include(GNUInstallDirs) # Installation directories for `install` command and pkgconfig file
find_package(PkgConfig)

set(CMAKE_CXX_STANDARD 17)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-W -Wextra -Wall -Wno-gnu -pedantic)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-W -Wextra -Wall -pedantic)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
endif()

set(LIB_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(LIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

set(LIB_PUBLIC_HEADERS
    ${LIB_INCLUDE_DIR}/database.h
    ${LIB_INCLUDE_DIR}/db_row.h
    ${LIB_INCLUDE_DIR}/connection.h
    ${LIB_INCLUDE_DIR}/statement.h
    ${LIB_INCLUDE_DIR}/selector.h
    ${LIB_INCLUDE_DIR}/transaction.h)

set(LIB_CPP_SOURCES
    ${LIB_SOURCE_DIR}/db_row.cpp
    ${LIB_SOURCE_DIR}/connection.cpp
    ${LIB_SOURCE_DIR}/selector.cpp
    ${LIB_SOURCE_DIR}/statement_impl.cpp
    ${LIB_SOURCE_DIR}/pq/connection_impl.cpp
    ${LIB_SOURCE_DIR}/pq/statement_impl.cpp
    ${LIB_SOURCE_DIR}/pq/statement_base_impl.cpp
    ${LIB_SOURCE_DIR}/pq/selector_impl.cpp
    ${LIB_SOURCE_DIR}/sqlite/sqlitedb.cpp)

set(LIB_SOURCES ${LIB_PUBLIC_HEADERS} ${LIB_CPP_SOURCES})

set(BACKEND_LIBRARIES "")
if (POSTGRESQL_BACKEND)
    find_package(PostgreSQL REQUIRED)
    add_definitions(-DPQ_BACKEND)
    set(BACKEND_LIBRARIES ${BACKEND_LIBRARIES} ${PostgreSQL_LIBRARIES})
endif()

if (SQLITE_BACKEND)
    find_package(SQLite3 REQUIRED)
    add_definitions(-DSQLITE_BACKEND)
endif()

add_library(sql-wrapper-objects OBJECT ${LIB_SOURCES})
set_target_properties(sql-wrapper-objects PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(sql-wrapper SHARED $<TARGET_OBJECTS:sql-wrapper-objects>)
add_library(sql-wrapper-static STATIC $<TARGET_OBJECTS:sql-wrapper-objects>)

set_target_properties(sql-wrapper
    PROPERTIES
    PUBLIC_HEADER "${LIB_PUBLIC_HEADERS}"
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    OUTPUT_NAME "sql-wrapper")

install(TARGETS sql-wrapper
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT libraries # This is for Windows
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT static_libraries
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIB_INCLUDE_SUBDIR} COMPONENT dev)

option(ENABLE_EXAMPLES "Build and install examples (default ON)" OFF)
if(ENABLE_EXAMPLES)
    add_subdirectory("${CMAKE_SOURCE_DIR}/examples")
endif()


if(WIN32 AND NOT CYGWIN)
    set(LIB_CONFIG_INSTALL_DIR CMake)
else()
    set(LIB_CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/SqlWrapper)
endif()

include(CMakePackageConfigHelpers)

configure_file(SqlWrapperConfig.cmake.in SqlWrapperConfig.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/SqlWrapperConfig.cmake DESTINATION ${LIB_CONFIG_INSTALL_DIR} COMPONENT dev)

write_basic_package_version_file(SqlWrapperConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_BINARY_DIR}/SqlWrapperConfigVersion.cmake DESTINATION ${LIB_CONFIG_INSTALL_DIR} COMPONENT dev)
