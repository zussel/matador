SET(SOURCES
  byte_buffer.cpp
  library.cpp
  blob.cpp
  calendar.cpp
  date.cpp
  time.cpp
  sequencer.cpp
  string.cpp
  strptime.cpp
  serializer.cpp
  file.cpp os.cpp
  buffer.cpp
  thread_pool.cpp
  base64.cpp
  buffer_view.cpp
  string_cursor.cpp
  sha256.cpp
  hmac.cpp
  sequence_synchronizer.cpp
  url.cpp
  html.cpp
  thread_helper.cpp
  identifier.cpp
  field_attributes.cpp
  constraints.cpp)

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
  ../../include/matador/utils/string.hpp
  ../../include/matador/utils/strptime.hpp
  ../../include/matador/utils/enable_if.hpp
  ../../include/matador/utils/conditional.hpp
  ../../include/matador/utils/serializer.hpp
  ../../include/matador/utils/basic_identifier.hpp
  ../../include/matador/utils/identifier_resolver.hpp
  ../../include/matador/utils/identifier_setter.hpp
  ../../include/matador/utils/identifiable_holder.hpp
  ../../include/matador/utils/any.hpp
  ../../include/matador/utils/any_visitor.hpp
  ../../include/matador/utils/is_builtin.hpp
  ../../include/matador/utils/is_varchar.hpp
  ../../include/matador/utils/varchar.hpp
  ../../include/matador/utils/file.hpp
  ../../include/matador/utils/os.hpp
  ../../include/matador/utils/buffer.hpp
  ../../include/matador/utils/tree.hpp
  ../../include/matador/utils/stream_processor.hpp
  ../../include/matador/utils/stream_processor.tpp
  ../../include/matador/utils/stream.hpp
  ../../include/matador/utils/optional.hpp
  ../../include/matador/utils/thread_pool.hpp
  ../../include/matador/utils/base64.hpp
  ../../include/matador/utils/buffer_view.hpp
  ../../include/matador/utils/string_cursor.hpp
  ../../include/matador/utils/sha256.hpp
  ../../include/matador/utils/hmac.hpp
  ../../include/matador/utils/sequence_synchronizer.hpp
  ../../include/matador/utils/url.hpp
  ../../include/matador/utils/html.hpp
  ../../include/matador/utils/thread_helper.hpp
  ../../include/matador/utils/identifier.hpp
        ../../include/matador/utils/contraints.hpp
        ../../include/matador/utils/field_attributes.hpp)

ADD_LIBRARY(matador-utils STATIC ${SOURCES} ${HEADER})

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
