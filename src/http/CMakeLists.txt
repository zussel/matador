SET(SOURCES
  http_client.cpp
  http_server.cpp
  request.cpp
  request_parser.cpp
  http_server_connection.cpp
  http.cpp
  response.cpp
  response_header.cpp
  request_header.cpp
  route_endpoint.cpp
  routing_engine.cpp
  default_responses.cpp
  mime_types.cpp
  static_file_service.cpp
  response_parser.cpp
  http_client_connection.cpp
  http_client_connection.cpp
  template_engine.cpp
  detail/template_command.cpp
  detail/template_command_factory.cpp
  detail/template_parser.cpp
  detail/template_part.cpp
  detail/template_expression.cpp
  middleware.cpp
  middleware/routing_middleware.cpp detail/template_filter.cpp detail/template_filter_factory.cpp)

SET(HEADER
  ../../include/matador/http/http_client.hpp
  ../../include/matador/http/http_server.hpp
  ../../include/matador/http/response_header.hpp
  ../../include/matador/http/response.hpp
  ../../include/matador/http/http.hpp
  ../../include/matador/http/request.hpp
  ../../include/matador/http/request_header.hpp
  ../../include/matador/http/request_parser.hpp
  ../../include/matador/http/http_server_connection.hpp
  ../../include/matador/http/route_endpoint.hpp
  ../../include/matador/http/routing_engine.hpp
  ../../include/matador/http/default_responses.hpp
  ../../include/matador/http/mime_types.hpp
  ../../include/matador/http/static_file_service.hpp
  ../../include/matador/http/response_parser.hpp
  ../../include/matador/http/http_client_connection.hpp
  ../../include/matador/http/http_client_connection.hpp
  ../../include/matador/http/template_engine.hpp
  ../../include/matador/http/detail/template_command.hpp
  ../../include/matador/http/detail/template_command_factory.hpp
  ../../include/matador/http/detail/template_parser.hpp
  ../../include/matador/http/detail/template_part.hpp detail/template_expression.cpp
  ../../include/matador/http/detail/template_expression.hpp
  ../../include/matador/http/middleware.hpp
  ../../include/matador/http/middleware/routing_middleware.hpp
  ../../include/matador/http/auth/jwt_token.hpp
  ../../include/matador/http/enum_class_hash.hpp ../../include/matador/http/detail/template_filter.hpp ../../include/matador/http/detail/template_filter_factory.hpp)

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
