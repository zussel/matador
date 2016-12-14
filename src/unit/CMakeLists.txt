SET(SOURCES
  test_suite.cpp
  unit_test.cpp
)

SET(HEADER
  ${CMAKE_SOURCE_DIR}/include/oos/unit/test_suite.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/unit/unit_exception.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/unit/unit_test.hpp
)

ADD_LIBRARY(oos-unit SHARED ${SOURCES} ${HEADER})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(oos-unit
                      PROPERTIES
                      VERSION 0.5.0
                      SOVERSION 0)

SOURCE_GROUP("include\\oos\\unit" FILES ${HEADER})
SOURCE_GROUP("src\\oos\\unit" FILES ${SOURCES})

INSTALL(
  TARGETS oos-unit
  ARCHIVE  DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS oos-unit
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER}
  DESTINATION include/oos/unit
  COMPONENT headers
)
