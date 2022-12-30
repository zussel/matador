SET(SOURCES
  json.cpp
  json_parser.cpp
  generic_json_parser.cpp
  json_mapper.cpp
  json_serializer.cpp
  json_mapper_serializer.cpp
  json_identifier_serializer.cpp
  json_format.cpp)

SET(HEADER
  ../../include/matador/json/json.hpp
  ../../include/matador/json/json_exception.hpp
  ../../include/matador/json/json_parser.hpp
  ../../include/matador/json/generic_json_parser.hpp
  ../../include/matador/json/json_mapper.hpp
  ../../include/matador/json/basic_json_mapper.hpp
  ../../include/matador/json/json_serializer.hpp
  ../../include/matador/json/json_mapper_serializer.hpp
  ../../include/matador/json/json_identifier_serializer.hpp
  ../../include/matador/json/json_format.hpp
  ../../include/matador/json/json_utils.hpp
  )

ADD_LIBRARY(matador-json STATIC ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-json matador-utils ${CMAKE_DL_LIBS})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-json
  PROPERTIES
  VERSION ${APP_VERSION}
  SOVERSION ${APP_MAJOR_VERSION})

SOURCE_GROUP("include\\matador\\json" FILES ${HEADER})
SOURCE_GROUP("src\\matador\\json" FILES ${SOURCES})

INSTALL(
  TARGETS matador-json
  ARCHIVE  DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS matador-json
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER}
  DESTINATION include/matador/json
  COMPONENT headers
)
