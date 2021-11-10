SET (SANDBOX_SOURCES sandbox.cpp ../src/utils/thread_helper.cpp ../include/matador/utils/thread_helper.hpp)

ADD_EXECUTABLE(sandbox ${SANDBOX_SOURCES})

ADD_EXECUTABLE(di di.cpp)

ADD_EXECUTABLE(reactor reactor.cpp leader_follower_thread_pool.cpp leader_follower_thread_pool.hpp)

TARGET_LINK_LIBRARIES(reactor
  matador-utils
  matador-logger
  matador-net
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
)

SET(NET_LIBRARIES)

IF(WIN32)
  MESSAGE(STATUS "Appending Windows Socket libs: wsock32 ws2_32")
  LIST(APPEND NET_LIBRARIES wsock32 ws2_32)
ENDIF()

TARGET_LINK_LIBRARIES(sandbox
    matador-unit
    matador-utils
    matador-json
    matador-logger
    matador-object
    matador-http
    matador-net
    matador-sql
    matador-orm
    ${CMAKE_DL_LIBS}
    ${MATADOR_DB_LIBRARIES}
    ${NET_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)
