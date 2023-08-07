SET (SANDBOX_SOURCES sandbox.cpp)

ADD_EXECUTABLE(sandbox ${SANDBOX_SOURCES})
ADD_EXECUTABLE(di di.cpp)
ADD_EXECUTABLE(reactor reactor.cpp)
ADD_EXECUTABLE(row row.cpp)
ADD_EXECUTABLE(identifier identifier.cpp)

TARGET_LINK_LIBRARIES(reactor
  matador-utils
  matador-logger
  matador-net
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
)

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

TARGET_LINK_LIBRARIES(row
    matador-unit
    matador-utils
    matador-json
    matador-logger
    matador-object
    matador-sql
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
)
