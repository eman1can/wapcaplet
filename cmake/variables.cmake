# ---- Developer mode ----

# Developer mode enables targets and code paths in the CMake scripts that are
# only relevant for the developer(s) of wapcaplet
# Targets necessary to build the project must be provided unconditionally, so
# consumers can trivially build and package the project
if(PROJECT_IS_TOP_LEVEL)
  option(wapcaplet_DEVELOPER_MODE "Enable developer mode" OFF)
  option(BUILD_SHARED_LIBS "Build shared libs." OFF)
endif()

# ---- Suppress C4251 on Windows ----

# Please see include/wapcaplet/wapcaplet.h for more details
set(pragma_suppress_c4251 "
/* This needs to suppress only for MSVC */
#if defined(_MSC_VER) && !defined(__ICL)
#  define WAPCAPLET_SUPPRESS_C4251 _Pragma(\"warning(suppress:4251)\")
#else
#  define WAPCAPLET_SUPPRESS_C4251
#endif
")

# ---- Warning guard ----

# target_include_directories with the SYSTEM modifier will request the compiler
# to omit warnings from the provided paths, if the compiler supports that
# This is to provide a user experience similar to find_package when
# add_subdirectory or FetchContent is used to consume this project
set(warning_guard "")
if(NOT PROJECT_IS_TOP_LEVEL)
  option(
      wapcaplet_INCLUDES_WITH_SYSTEM
      "Use SYSTEM modifier for wapcaplet's includes, disabling warnings"
      ON
  )
  mark_as_advanced(wapcaplet_INCLUDES_WITH_SYSTEM)
  if(wapcaplet_INCLUDES_WITH_SYSTEM)
    set(warning_guard SYSTEM)
  endif()
endif()
