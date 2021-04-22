SET(NET_LIBRARIES)

IF(WIN32)
  MESSAGE(STATUS "Appending Windows Socket libs: wsock32 ws2_32")
  LIST(APPEND NET_LIBRARIES wsock32 ws2_32)
ENDIF()

SET (DEMO_SOURCES
  main.cpp)

ADD_EXECUTABLE(demo ${DEMO_SOURCES})

CONFIGURE_FILE(html/login.html ${PROJECT_BINARY_DIR}/bin/html/login.html COPYONLY)
CONFIGURE_FILE(html/secret.html ${PROJECT_BINARY_DIR}/bin/html/secret.html COPYONLY)

TARGET_LINK_LIBRARIES(demo
  matador-unit
  matador-utils
  matador-logger
  matador-http
  matador-net
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
  ${NET_LIBRARIES}
)
