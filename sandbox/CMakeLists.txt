SET (SANDBOX_SOURCES sandbox.cpp
  object_observer_producer.hpp)

ADD_EXECUTABLE(sandbox ${SANDBOX_SOURCES})
ADD_EXECUTABLE(datetime datetime.cpp)

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

TARGET_LINK_LIBRARIES(datetime
    matador-utils
)
