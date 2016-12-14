SET(SOURCES
  byte_buffer.cpp
  library.cpp
  blob.cpp
  calendar.cpp
  date.cpp
  time.cpp
  varchar.cpp
  sequencer.cpp
  string.cpp
  strptime.cpp
  basic_identifier.cpp
  basic_identifier_serializer.cpp
  serializer.cpp
)

SET(HEADER
  ${CMAKE_SOURCE_DIR}/include/oos/utils/access.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/algorithm.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/base_class.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/byte_buffer.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/cascade_type.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/singleton.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/library.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/blob.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/calendar.h
  ${CMAKE_SOURCE_DIR}/include/oos/utils/date.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/time.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/varchar.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/sequencer.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/factory.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/string.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/strptime.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/enable_if.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/conditional.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/basic_identifier.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/basic_identifier_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/identifier.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/identifier_resolver.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/identifier_setter.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/identifiable_holder.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/any.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/utils/any_visitor.hpp
)

ADD_LIBRARY(oos-utils SHARED ${SOURCES} ${HEADER})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(oos-utils
  PROPERTIES
  VERSION 0.5.0
  SOVERSION 0)

SOURCE_GROUP("include\\oos\\utils" FILES ${HEADER})
SOURCE_GROUP("src\\oos\\utils" FILES ${SOURCES})

INSTALL(
  TARGETS oos-utils
  ARCHIVE  DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS oos-utils
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER}
  DESTINATION include/oos/utils
  COMPONENT headers
)
