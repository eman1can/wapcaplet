cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    wapcaplet
    VERSION 1.0.0
    DESCRIPTION "Short description"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    wapcaplet_wapcaplet
        src/wapcaplet.cpp
)
add_library(wapcaplet::wapcaplet ALIAS wapcaplet_wapcaplet)

include(GenerateExportHeader)
generate_export_header(
    wapcaplet_wapcaplet
    BASE_NAME wapcaplet
    EXPORT_FILE_NAME export/wapcaplet/wapcaplet_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(wapcaplet_wapcaplet PUBLIC WAPCAPLET_STATIC_DEFINE)
endif()

set_target_properties(
    wapcaplet_wapcaplet PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME wapcaplet
    OUTPUT_NAME wapcaplet
)

target_include_directories(
    wapcaplet_wapcaplet ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    wapcaplet_wapcaplet SYSTEM
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(wapcaplet_wapcaplet PUBLIC cxx_std_20)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT wapcaplet_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of wapcaplet"
  )
endif()

include(cmake/dev-mode.cmake)
