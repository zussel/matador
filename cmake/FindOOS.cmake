# - Find OOS
# Find the OOS includes and client library
# This module defines
#  OOS_INCLUDE_DIR, where to find oos/object/object_store.hpp
#  OOS_LIBRARIES, the libraries needed to use OOS.
#  OOS_FOUND, If false, do not try to use OOS.

if(OOS_INCLUDE_DIR AND OOS_LIBRARIES)
   set(OOS_FOUND TRUE)

else(OOS_INCLUDE_DIR AND OOS_LIBRARIES)

  find_path(OOS_INCLUDE_DIR oos/object/object_store.hpp
      /usr/include/
      /usr/local/include/
      /usr/include/oos/
      /usr/local/include/oos/
      /opt/oos/include/
      $ENV{ProgramFiles}/OOS/*/include
      $ENV{SystemDrive}/OOS/*/include
  )

  if(WIN32)
    find_library(OOS_LIBRARIES NAMES oos-object oos-utils oos-unit oos-orm oos-sql
      PATHS
      $ENV{ProgramFiles}/OOS/*/lib
      $ENV{SystemDrive}/OOS/*/lib
    )
  else(WIN32)
    find_library(OOS_LIBRARIES NAMES oos-object
      PATHS
      /usr/lib
      /usr/lib/oos
      /usr/local/lib
      /usr/local/lib/oos
      /opt/oos/lib
    )
  endif(WIN32)

  if(OOS_INCLUDE_DIR AND OOS_LIBRARIES)
    set(OOS_FOUND TRUE)
    message(STATUS "Found OOS: ${OOS_INCLUDE_DIR}, ${OOS_LIBRARIES}")
  else(OOS_INCLUDE_DIR AND OOS_LIBRARIES)
    set(OOS_FOUND FALSE)
    if (OOS_FIND_REQUIRED)
      message(FATAL_ERROR "OOS not found.")
    else (OOS_FIND_REQUIRED)
      message(STATUS "OOS not found.")
    endif (OOS_FIND_REQUIRED)
  endif(OOS_INCLUDE_DIR AND OOS_LIBRARIES)

  mark_as_advanced(OOS_INCLUDE_DIR OOS_LIBRARIES)

endif(OOS_INCLUDE_DIR AND OOS_LIBRARIES)
