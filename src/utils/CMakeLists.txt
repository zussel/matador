SET(SOURCES
  base64.cpp
  buffer.cpp
  buffer_view.cpp
  byte_buffer.cpp
  calendar.cpp
  constraints.cpp
  convert.cpp
  date.cpp
  field_attributes.cpp
  file.cpp
  foreign_attributes.cpp
  hmac.cpp
  html.cpp
  library.cpp
  os.cpp
  sequence_synchronizer.cpp
  sequencer.cpp
  sha256.cpp
  string.cpp
  string_cursor.cpp
  strptime.cpp
  thread_helper.cpp
  thread_pool.cpp
  time.cpp
  url.cpp
  version.cpp
)

SET(HEADER
  ../../include/matador/utils/access.hpp
  ../../include/matador/utils/algorithm.hpp
  ../../include/matador/utils/base64.hpp
  ../../include/matador/utils/base_class.hpp
  ../../include/matador/utils/buffer.hpp
  ../../include/matador/utils/buffer_view.hpp
  ../../include/matador/utils/byte_buffer.hpp
  ../../include/matador/utils/calendar.h
  ../../include/matador/utils/cascade_type.hpp
  ../../include/matador/utils/contraints.hpp
  ../../include/matador/utils/convert.hpp
  ../../include/matador/utils/data_types.hpp
  ../../include/matador/utils/date.hpp
  ../../include/matador/utils/di.hpp
  ../../include/matador/utils/enum_mapper.hpp
  ../../include/matador/utils/export.hpp
  ../../include/matador/utils/factory.hpp
  ../../include/matador/utils/fetch_type.hpp
  ../../include/matador/utils/field_attributes.hpp
  ../../include/matador/utils/file.hpp
  ../../include/matador/utils/foreign_attributes.hpp
  ../../include/matador/utils/hmac.hpp
  ../../include/matador/utils/html.hpp
  ../../include/matador/utils/is_builtin.hpp
  ../../include/matador/utils/library.hpp
  ../../include/matador/utils/os.hpp
  ../../include/matador/utils/result.hpp
  ../../include/matador/utils/sequence_synchronizer.hpp
  ../../include/matador/utils/sequencer.hpp
  ../../include/matador/utils/sha256.hpp
  ../../include/matador/utils/singleton.hpp
  ../../include/matador/utils/stream.hpp
  ../../include/matador/utils/stream_processor.hpp
  ../../include/matador/utils/stream_processor.tpp
  ../../include/matador/utils/string.hpp
  ../../include/matador/utils/string_cursor.hpp
  ../../include/matador/utils/strptime.hpp
  ../../include/matador/utils/thread_helper.hpp
  ../../include/matador/utils/thread_pool.hpp
  ../../include/matador/utils/time.hpp
  ../../include/matador/utils/tree.hpp
  ../../include/matador/utils/types.hpp
  ../../include/matador/utils/url.hpp
  ../../include/matador/utils/version.hpp
  ../../include/matador/utils/macro_map.hpp
)

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
