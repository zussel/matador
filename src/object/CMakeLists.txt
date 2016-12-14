SET(SOURCES
  action.cpp
  object_store.cpp
  object_proxy.cpp
  object_serializer.cpp
  prototype_node.cpp
  identifier.cpp
  prototype_iterator.cpp
  object_holder.cpp
  transaction.cpp
  action_inserter.cpp
  action_remover.cpp
  insert_action.cpp
  update_action.cpp
  delete_action.cpp
  basic_has_many_item.cpp
  object_proxy_accessor.cpp
)

SET(HEADER
  ${CMAKE_SOURCE_DIR}/include/oos/object/action.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/attribute_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/generic_access.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/object_exception.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/object_ptr.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/object_store.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/object_view.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/object_proxy.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/object_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/prototype_node.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/object_observer.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/object_expression.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/attribute_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/prototype_iterator.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/has_many.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/has_one.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/container_type_traits.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/object_holder.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/basic_has_many.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/has_many_vector.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/has_many_list.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/has_many_set.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/transaction.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/action_inserter.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/action_visitor.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/action_remover.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/insert_action.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/update_action.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/delete_action.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/abstract_has_many.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/has_many_item.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/basic_has_many_item.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/identifier_proxy_map.hpp
  ${CMAKE_SOURCE_DIR}/include/oos/object/object_proxy_accessor.hpp)

ADD_LIBRARY(oos-object SHARED ${SOURCES} ${HEADER})

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(oos-object
  PROPERTIES
  VERSION 0.5.0
  SOVERSION 0)

SOURCE_GROUP("include\\oos\\object" FILES ${HEADER})
SOURCE_GROUP("src\\oos\\object" FILES ${SOURCES})

INSTALL(
  TARGETS oos-object
  ARCHIVE  DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS oos-object
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER}
  DESTINATION include/oos/object
  COMPONENT headers
)
