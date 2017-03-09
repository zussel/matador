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
  identifier.cpp
  basic_identifier.cpp
  basic_identifier_serializer.cpp
  serializer.cpp
)

SET(HEADER
  ${CMAKE_SOURCE_DIR}/include/matador/utils/access.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/algorithm.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/base_class.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/byte_buffer.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/cascade_type.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/singleton.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/library.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/blob.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/calendar.h
  ${CMAKE_SOURCE_DIR}/include/matador/utils/date.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/time.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/varchar.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/sequencer.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/factory.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/string.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/strptime.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/enable_if.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/conditional.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/basic_identifier.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/basic_identifier_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/identifier.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/identifier_resolver.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/identifier_setter.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/identifiable_holder.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/any.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/utils/any_visitor.hpp
)

ADD_LIBRARY(matador-utils SHARED ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-utils ${CMAKE_DL_LIBS})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-utils
  PROPERTIES
  VERSION 0.5.0
  SOVERSION 0)

SOURCE_GROUP("include\\matador\\utils" FILES ${HEADER})
SOURCE_GROUP("src\\matador\\utils" FILES ${SOURCES})

INSTALL(
  TARGETS matador-utils
  ARCHIVE  DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS matador-utils
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER}
  DESTINATION include/matador/utils
  COMPONENT headers
)
