set(wapcaplet_FOUND YES)

include(CMakeFindDependencyMacro)
find_dependency(fmt)

if(wapcaplet_FOUND)
  include("${CMAKE_CURRENT_LIST_DIR}/wapcapletTargets.cmake")
endif()
