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
  pages/movie_page.cpp
  pages/movie_page.hpp
  models/movie.hpp
  models/person.hpp
  services/movie_service.cpp
  services/movie_service.hpp pages/main_page.cpp pages/main_page.hpp)

ADD_EXECUTABLE(demo ${DEMO_SOURCES})

add_custom_command(TARGET demo PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/examples/demo/web/ $<TARGET_FILE_DIR:demo>/web)

add_custom_command(TARGET demo PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/examples/demo/templates/ $<TARGET_FILE_DIR:demo>/templates)

TARGET_LINK_LIBRARIES(demo
  matador-utils
  matador-logger
  matador-sql
  matador-object
  matador-orm
  matador-net
  matador-http
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
  ${NET_LIBRARIES}
  ${SQLITE3_LIBRARY}
)
