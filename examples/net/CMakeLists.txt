SET(NET_LIBRARIES)

IF(WIN32)
  MESSAGE(STATUS "Appending Windows Socket libs: wsock32 ws2_32")
  LIST(APPEND NET_LIBRARIES wsock32 ws2_32)
ENDIF()

# echo server example

SET (ECHO_SERVER_SOURCES
  echo_server_main.cpp
  echo_server_connection.cpp
  echo_server_connection.hpp
  echo_server.cpp echo_server.hpp)

ADD_EXECUTABLE(echo_server ${ECHO_SERVER_SOURCES})

TARGET_LINK_LIBRARIES(echo_server
  matador-unit
  matador-utils
  matador-json
  matador-logger
  matador-net
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
  ${NET_LIBRARIES}
)

# echo client example

SET (ECHO_CLIENT_SOURCES
  echo_client_main.cpp echo_client_connection.cpp echo_client_connection.hpp echo_client.cpp echo_client.hpp)

ADD_EXECUTABLE(echo_client ${ECHO_CLIENT_SOURCES})

TARGET_LINK_LIBRARIES(echo_client
  matador-unit
  matador-utils
  matador-json
  matador-logger
  matador-net
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
  ${NET_LIBRARIES}
)
