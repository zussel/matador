SET(SOURCES
  byte_buffer.cpp
  library.cpp
  blob.cpp
  calendar.cpp
  date.cpp
  json.cpp
  json_parser.cpp
  time.cpp
  sequencer.cpp
  string.cpp
  strptime.cpp
  identifier.cpp
  basic_identifier.cpp
  basic_identifier_serializer.cpp
  serializer.cpp
  generic_json_parser.cpp json_mapper.cpp file.cpp os.cpp)

SET(HEADER
  ../../include/matador/utils/access.hpp
  ../../include/matador/utils/algorithm.hpp
  ../../include/matador/utils/base_class.hpp
  ../../include/matador/utils/byte_buffer.hpp
  ../../include/matador/utils/cascade_type.hpp
  ../../include/matador/utils/singleton.hpp
  ../../include/matador/utils/library.hpp
  ../../include/matador/utils/blob.hpp
  ../../include/matador/utils/calendar.h
  ../../include/matador/utils/date.hpp
  ../../include/matador/utils/time.hpp
  ../../include/matador/utils/sequencer.hpp
  ../../include/matador/utils/factory.hpp
  ../../include/matador/utils/json.hpp
  ../../include/matador/utils/json_exception.hpp
  ../../include/matador/utils/json_parser.hpp
  ../../include/matador/utils/string.hpp
  ../../include/matador/utils/strptime.hpp
  ../../include/matador/utils/enable_if.hpp
  ../../include/matador/utils/conditional.hpp
  ../../include/matador/utils/serializer.hpp
  ../../include/matador/utils/basic_identifier.hpp
  ../../include/matador/utils/basic_identifier_serializer.hpp
  ../../include/matador/utils/identifier.hpp
  ../../include/matador/utils/identifier_resolver.hpp
  ../../include/matador/utils/identifier_setter.hpp
  ../../include/matador/utils/identifiable_holder.hpp
  ../../include/matador/utils/any.hpp
  ../../include/matador/utils/any_visitor.hpp
  ../../include/matador/utils/is_builtin.hpp
  ../../include/matador/utils/memory.hpp
  ../../include/matador/utils/is_varchar.hpp
  ../../include/matador/utils/varchar.hpp
    ../../include/matador/utils/generic_json_parser.hpp
    ../../include/matador/utils/json_mapper.hpp ../../include/matador/utils/basic_json_mapper.hpp ../../include/matador/utils/file.hpp ../../include/matador/utils/os.hpp)

ADD_LIBRARY(matador-utils SHARED ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-utils ${CMAKE_DL_LIBS})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-utils
  PROPERTIES
  VERSION ${APP_VERSION}
  SOVERSION ${APP_MAJOR_VERSION})

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
