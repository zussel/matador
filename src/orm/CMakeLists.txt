SET(SOURCES
  persistence.cpp
  session.cpp
  basic_table.cpp
        )

SET(HEADER
  ${CMAKE_SOURCE_DIR}/include/matador/orm/persistence.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/orm/table.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/orm/session.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/orm/basic_table.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/orm/identifier_binder.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/orm/identifier_column_resolver.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/orm/relation_resolver.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/orm/relation_resolver.tpp
  ${CMAKE_SOURCE_DIR}/include/matador/orm/relation_item_appender.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/orm/persistence_observer.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/orm/persistence_observer.tpp
  ${CMAKE_SOURCE_DIR}/include/matador/orm/basic_relation_data.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/orm/belongs_to_resolver.hpp)

ADD_LIBRARY(matador-orm SHARED ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-orm matador-utils matador-sql matador-object)

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-orm
  PROPERTIES
  VERSION 0.5.0
  SOVERSION 0)

SOURCE_GROUP("include\\matador\\orm" FILES ${HEADER})
SOURCE_GROUP("src\\matador\\orm" FILES ${SOURCES})

INSTALL(
  TARGETS matador-orm
  ARCHIVE  DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS matador-orm
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER}
  DESTINATION include/matador/orm
  COMPONENT headers
)
