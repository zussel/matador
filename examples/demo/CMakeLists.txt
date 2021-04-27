SET(NET_LIBRARIES)

IF(WIN32)
  MESSAGE(STATUS "Appending Windows Socket libs: wsock32 ws2_32")
  LIST(APPEND NET_LIBRARIES wsock32 ws2_32)
ENDIF()

SET (DEMO_SOURCES
  main.cpp
  services/auth_service.cpp
  services/auth_service.hpp
  models/credential.hpp
  models/user.hpp
  pages/main_page.cpp
  pages/main_page.hpp
)

ADD_EXECUTABLE(demo ${DEMO_SOURCES})

CONFIGURE_FILE(web/css/bulma.min.css ${PROJECT_BINARY_DIR}/bin/web/css/bulma.min.css COPYONLY)
CONFIGURE_FILE(web/js/axios.min-0.21.1.js ${PROJECT_BINARY_DIR}/bin/web/js/axios.min-0.21.1.js COPYONLY)
CONFIGURE_FILE(web/js/demo.js ${PROJECT_BINARY_DIR}/bin/web/js/demo.js COPYONLY)
CONFIGURE_FILE(web/login.html ${PROJECT_BINARY_DIR}/bin/web/login.html COPYONLY)
CONFIGURE_FILE(web/secure.html ${PROJECT_BINARY_DIR}/bin/web/secure.html COPYONLY)
CONFIGURE_FILE(web/index.html ${PROJECT_BINARY_DIR}/bin/web/index.html COPYONLY)

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
