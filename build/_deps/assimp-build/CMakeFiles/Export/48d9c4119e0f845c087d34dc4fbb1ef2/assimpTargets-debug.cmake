#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "assimp::assimp" for configuration "Debug"
set_property(TARGET assimp::assimp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(assimp::assimp PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libassimpd.so.5.4.3"
  IMPORTED_SONAME_DEBUG "libassimpd.so.5"
  )

list(APPEND _cmake_import_check_targets assimp::assimp )
list(APPEND _cmake_import_check_files_for_assimp::assimp "${_IMPORT_PREFIX}/lib/libassimpd.so.5.4.3" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
