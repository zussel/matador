SET(SOURCES
  logger.cpp
  log_manager.cpp basic_file_sink.cpp file_sink.cpp log_domain.cpp rotating_file_sink.cpp)

SET(HEADER
  ${CMAKE_SOURCE_DIR}/include/matador/logger/logger.hpp
  ../../include/matador/logger/log_manager.hpp ../../include/matador/logger/log_sink.hpp ../../include/matador/logger/basic_file_sink.hpp ../../include/matador/logger/file_sink.hpp ../../include/matador/logger/log_domain.hpp ../../include/matador/logger/log_level.hpp ../../include/matador/logger/rotating_file_sink.hpp)

ADD_LIBRARY(matador-logger SHARED ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-logger matador-utils)

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-logger
                      PROPERTIES
                      VERSION ${APP_VERSION}
                      SOVERSION ${APP_MAJOR_VERSION})

SOURCE_GROUP("include\\matador\\logger" FILES ${HEADER})
SOURCE_GROUP("src\\matador\\logger" FILES ${SOURCES})

INSTALL(
  TARGETS matador-logger
  ARCHIVE  DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS matador-logger
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER}
  DESTINATION include/matador/logger
  COMPONENT headers
)
