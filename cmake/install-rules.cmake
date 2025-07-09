if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/wapcaplet-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
# should match the name of variable set in the install-config.cmake script
set(package wapcaplet)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT wapcaplet_Development
)

install(
    TARGETS wapcaplet_wapcaplet
    EXPORT wapcapletTargets
    RUNTIME #
    COMPONENT wapcaplet_Runtime
    LIBRARY #
    COMPONENT wapcaplet_Runtime
    NAMELINK_COMPONENT wapcaplet_Development
    ARCHIVE #
    COMPONENT wapcaplet_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    wapcaplet_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE wapcaplet_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(wapcaplet_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${wapcaplet_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT wapcaplet_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${wapcaplet_INSTALL_CMAKEDIR}"
    COMPONENT wapcaplet_Development
)

install(
    EXPORT wapcapletTargets
    NAMESPACE wapcaplet::
    DESTINATION "${wapcaplet_INSTALL_CMAKEDIR}"
    COMPONENT wapcaplet_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
