SET(NET_LIBRARIES)

IF(WIN32)
  MESSAGE(STATUS "Appending Windows Socket libs: wsock32 ws2_32")
  LIST(APPEND NET_LIBRARIES wsock32 ws2_32)
ENDIF()

SET (HTTP_SERVER_SOURCES
  main.cpp)

ADD_EXECUTABLE(http_server ${HTTP_SERVER_SOURCES})

TARGET_LINK_LIBRARIES(http_server
  matador-unit
  matador-utils
  matador-logger
  matador-http
  matador-net
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
  ${NET_LIBRARIES}
)

SET (HTML_BUILDER_SOURCES
  html_builder_main.cpp)

ADD_EXECUTABLE(html_builder_server ${HTML_BUILDER_SOURCES})

TARGET_LINK_LIBRARIES(html_builder_server
  matador-unit
  matador-utils
  matador-logger
  matador-http
  matador-net
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
  ${NET_LIBRARIES}
)
