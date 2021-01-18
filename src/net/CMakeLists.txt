SET(SOURCES
  address.cpp fdset.cpp select_fdsets.cpp handler.cpp acceptor.cpp reactor.cpp os.cpp error.cpp connector.cpp address_resolver.cpp io_service.cpp stream_handler.cpp socket_interrupter.cpp)

SET(HEADER
  ../../include/matador/net/socket.hpp
  ../../include/matador/net/socket.tpp
  ../../include/matador/net/peer.hpp
  ../../include/matador/net/address.hpp
  ../../include/matador/net/fdset.hpp
  ../../include/matador/net/select_fdsets.hpp
  ../../include/matador/net/handler.hpp
  ../../include/matador/net/acceptor.hpp
  ../../include/matador/net/reactor.hpp
  ../../include/matador/net/os.hpp
  ../../include/matador/net/error.hpp
  ../../include/matador/net/event_type.hpp
  ../../include/matador/net/socket_acceptor.hpp
  ../../include/matador/net/socket_stream.hpp
  ../../include/matador/net/connector.hpp
  ../../include/matador/net/address_resolver.hpp
  ../../include/matador/net/io_service.hpp
  ../../include/matador/net/stream_handler.hpp
  ../../include/matador/net/io_stream.hpp
  ../../include/matador/net/socket_interrupter.hpp
  ../../include/matador/net/os.hpp ../../include/matador/net/handler_creator.hpp)

ADD_LIBRARY(matador-net SHARED ${SOURCES} ${HEADER})

if(WIN32)
  TARGET_LINK_LIBRARIES(matador-net matador-logger matador-utils wsock32 ws2_32)
else()
  TARGET_LINK_LIBRARIES(matador-net matador-logger matador-utils)
endif()

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
