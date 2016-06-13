find_path(SNAPPY_INCLUDE_DIR
    NAMES snappy.h
    PATHS ${SNAPPY_ROOT_DIR} ${SNAPPY_ROOT_DIR}/include
)
# mark_as_advanced(SNAPPY_INCLUDE_DIR)

find_library(SNAPPY_LIBRARIES 
    NAMES snappy
    PATHS ${SNAPPY_ROOT_DIR} ${SNAPPY_ROOT_DIR}/lib
)
# mark_as_advanced(SNAPPY_LIBRARIES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Snappy DEFAULT_MSG 
    SNAPPY_INCLUDE_DIR 
    SNAPPY_LIBRARIES
)

if(SNAPPY_FOUND)
  # message(STATUS "Found Snappy  (include: ${SNAPPY_INCLUDE_DIR}, library: ${SNAPPY_LIBRARIES})")
  mark_as_advanced(SNAPPY_INCLUDE_DIR SNAPPY_LIBRARIES)
endif()