SET(NET_LIBRARIES)

IF(WIN32)
  MESSAGE(STATUS "Appending Windows Socket libs: wsock32 ws2_32")
  LIST(APPEND NET_LIBRARIES wsock32 ws2_32)
ENDIF()

SET (HTTP_SERVER_SOURCES
  main.cpp
)

ADD_EXECUTABLE(http_server ${HTTP_SERVER_SOURCES})

TARGET_LINK_LIBRARIES(http_server
  matador-unit
  matador-utils
  matador-logger
  matador-http
  matador-net
  ${CMAKE_DL_LIBS}
  ${NET_LIBRARIES}
)
