SET(SOURCES
  acceptor.cpp
  address.cpp
  address_resolver.cpp
  connector.cpp
  error.cpp
  fdset.cpp
  handler.cpp
  io_service.cpp
  leader_follower_thread_pool.cpp
  os.cpp
  reactor.cpp
  select_fdsets.cpp
  socket_interrupter.cpp
  stream_handler.cpp
)

SET(HEADER
  ../../include/matador/net/acceptor.hpp
  ../../include/matador/net/address.hpp
  ../../include/matador/net/address_resolver.hpp
  ../../include/matador/net/connector.hpp
  ../../include/matador/net/error.hpp
  ../../include/matador/net/event_type.hpp
  ../../include/matador/net/export.hpp
  ../../include/matador/net/fdset.hpp
  ../../include/matador/net/handler.hpp
  ../../include/matador/net/handler_creator.hpp
  ../../include/matador/net/io_service.hpp
  ../../include/matador/net/io_stream.hpp
  ../../include/matador/net/leader_follower_thread_pool.hpp
  ../../include/matador/net/os.hpp
  ../../include/matador/net/os.hpp
  ../../include/matador/net/peer.hpp
  ../../include/matador/net/reactor.hpp
  ../../include/matador/net/select_fdsets.hpp
  ../../include/matador/net/socket.hpp
  ../../include/matador/net/socket.tpp
  ../../include/matador/net/socket_acceptor.hpp
  ../../include/matador/net/socket_interrupter.hpp
  ../../include/matador/net/socket_stream.hpp
  ../../include/matador/net/stream_handler.hpp
)

ADD_LIBRARY(matador-net STATIC ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-net matador-logger matador-utils ${NET_LIBRARIES})

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
