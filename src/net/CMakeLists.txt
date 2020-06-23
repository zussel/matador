SET(SOURCES
  socket.cpp address.cpp)

SET(HEADER
  ../../include/matador/net/socket.hpp
  ../../include/matador/net/socket.tpp
  ../../include/matador/net/peer.hpp
  ../../include/matador/net/address.hpp)

ADD_LIBRARY(matador-net SHARED ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-net matador-logger matador-utils)

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-net
  PROPERTIES
  VERSION ${APP_VERSION}
  SOVERSION ${APP_MAJOR_VERSION})

SOURCE_GROUP("include\\matador\\net" FILES ${HEADER})
SOURCE_GROUP("src\\matador\\net" FILES ${SOURCES})

INSTALL(
  TARGETS matador-net
  ARCHIVE  DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS matador-net
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER}
  DESTINATION include/matador/net
  COMPONENT headers
)
