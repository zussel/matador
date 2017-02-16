SET(SOURCES
  persistence.cpp
  session.cpp
  basic_table.cpp)

SET(HEADER
  ${CMAKE_SOURCE_DIR}/include/oos/orm/persistence.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/orm/table.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/orm/session.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/orm/basic_table.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/orm/identifier_binder.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/orm/identifier_column_resolver.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/orm/relation_table.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/orm/relation_resolver.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/orm/relation_item_appender.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/orm/persistence_observer.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/orm/persistence_observer.tpp
)

ADD_LIBRARY(oos-orm SHARED ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(oos-orm oos-utils oos-sql oos-object)

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(oos-orm
  PROPERTIES
  VERSION 0.5.0
  SOVERSION 0)

SOURCE_GROUP("include\\oos\\orm" FILES ${HEADER})
SOURCE_GROUP("src\\oos\\orm" FILES ${SOURCES})

INSTALL(
  TARGETS oos-orm
  ARCHIVE  DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS oos-orm
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER}
  DESTINATION include/oos/orm
  COMPONENT headers
)
