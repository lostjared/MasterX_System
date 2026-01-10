#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libmxcmd::mxcmd" for configuration "Debug"
set_property(TARGET libmxcmd::mxcmd APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libmxcmd::mxcmd PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libmxcmd.a"
  )

list(APPEND _cmake_import_check_targets libmxcmd::mxcmd )
list(APPEND _cmake_import_check_files_for_libmxcmd::mxcmd "${_IMPORT_PREFIX}/lib/libmxcmd.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
