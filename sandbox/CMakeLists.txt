SET (SANDBOX_SOURCES sandbox.cpp
  column.hpp
  row.hpp
  column_printer.hpp
  )

ADD_EXECUTABLE(sandbox ${SANDBOX_SOURCES})
ADD_EXECUTABLE(di di.cpp)
ADD_EXECUTABLE(reactor reactor.cpp)
ADD_EXECUTABLE(row row.cpp)
ADD_EXECUTABLE(identifier identifier.cpp)
ADD_EXECUTABLE(field_attribute field_attribute.cpp)
ADD_EXECUTABLE(sqlng sqlng.cpp)
ADD_EXECUTABLE(container container.cpp)
ADD_EXECUTABLE(connection_pool connection_pool.cpp)
ADD_EXECUTABLE(datetime datetime.cpp)
ADD_EXECUTABLE(query query.cpp)

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

TARGET_LINK_LIBRARIES(datetime
    matador-utils
)

TARGET_LINK_LIBRARIES(query
    matador-sql
    matador-logger
    matador-utils
)

TARGET_LINK_LIBRARIES(query
    matador-sql
    matador-logger
    matador-utils
)

TARGET_LINK_LIBRARIES(query
    matador-sql
    matador-logger
    matador-utils
)
