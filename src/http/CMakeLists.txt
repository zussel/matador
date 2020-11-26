SET(SOURCES
  http_client.cpp
  http_server.cpp
  request.cpp
  request_parser.cpp
  http_connection.cpp
  http.cpp
  response.cpp
  response_header.cpp request_header.cpp route_endpoint.cpp route_engine.cpp default_responses.cpp mime_types.cpp)

SET(HEADER
  ../../include/matador/http/http_client.hpp
  ../../include/matador/http/http_server.hpp
  ../../include/matador/http/response_header.hpp
  ../../include/matador/http/response.hpp
  ../../include/matador/http/http.hpp
  ../../include/matador/http/request.hpp
  ../../include/matador/http/request_header.hpp
  ../../include/matador/http/request_parser.hpp
  ../../include/matador/http/http_connection.hpp
  ../../include/matador/http/route_endpoint.hpp ../../include/matador/http/route_engine.hpp ../../include/matador/http/default_responses.hpp ../../include/matador/http/mime_types.hpp)

ADD_LIBRARY(matador-http SHARED ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-http matador-net matador-logger matador-utils)

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-http
                      PROPERTIES
                      VERSION ${APP_VERSION}
                      SOVERSION ${APP_MAJOR_VERSION})

SOURCE_GROUP("include\\matador\\http" FILES ${HEADER})
SOURCE_GROUP("src\\matador\\http" FILES ${SOURCES})

INSTALL(
  TARGETS matador-http
  ARCHIVE  DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS matador-http
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER}
  DESTINATION include/matador/logger
  COMPONENT headers
)
