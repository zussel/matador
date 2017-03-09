SET(SOURCES
  test_suite.cpp
  unit_test.cpp
)

SET(HEADER
  ${CMAKE_SOURCE_DIR}/include/matador/unit/test_suite.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/unit/unit_exception.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/unit/unit_test.hpp
)

ADD_LIBRARY(matador-unit SHARED ${SOURCES} ${HEADER})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-unit
                      PROPERTIES
                      VERSION 0.5.0
                      SOVERSION 0)

SOURCE_GROUP("include\\matador\\unit" FILES ${HEADER})
SOURCE_GROUP("src\\matador\\unit" FILES ${SOURCES})

INSTALL(
  TARGETS matador-unit
  ARCHIVE  DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS matador-unit
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER}
  DESTINATION include/matador/unit
  COMPONENT headers
)
