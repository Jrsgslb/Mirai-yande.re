#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "mirai-cpp" for configuration "Debug"
set_property(TARGET mirai-cpp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(mirai-cpp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/mirai-cpp.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS mirai-cpp )
list(APPEND _IMPORT_CHECK_FILES_FOR_mirai-cpp "${_IMPORT_PREFIX}/lib/mirai-cpp.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
