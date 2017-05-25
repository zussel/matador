SET(SOURCES
  action.cpp
  object_store.cpp
  object_proxy.cpp
  object_serializer.cpp
  prototype_node.cpp
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
  object_deleter.cpp
  object_inserter.cpp
  relation_field_endpoint.cpp relation_field_serializer.cpp abstract_has_many.cpp basic_has_many_to_many_item.cpp)

SET(HEADER
  ${CMAKE_SOURCE_DIR}/include/matador/object/action.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/attribute_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/generic_access.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/object_exception.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/object_ptr.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/object_store.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/object_view.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/object_proxy.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/object_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/prototype_node.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/object_store_observer.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/object_expression.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/attribute_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/prototype_iterator.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/has_many.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/has_one.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/container_type_traits.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/object_holder.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/basic_has_many.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/has_many_vector.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/has_many_list.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/has_many_set.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/transaction.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/action_inserter.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/action_visitor.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/action_remover.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/insert_action.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/update_action.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/delete_action.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/abstract_has_many.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/has_many_item.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/basic_has_many_item.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/identifier_proxy_map.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/object_proxy_accessor.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/belongs_to.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/object_holder_type.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/object_inserter.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/object_inserter.tpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/object_deleter.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/object_deleter.tpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/node_analyzer.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/node_analyzer.tpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/relation_field_endpoint.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/relation_field_endpoint.tpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/relation_field_serializer.hpp
  ${CMAKE_SOURCE_DIR}/include/matador/object/relation_field_serializer.tpp
  ../../include/matador/object/has_many_to_many_item.hpp ../../include/matador/object/basic_has_many_to_many_item.hpp ../../include/matador/object/has_many_inserter.hpp ../../include/matador/object/has_many_deleter.hpp ../../include/matador/object/has_many_item_holder.hpp ../../include/matador/object/prototype_info.hpp)

ADD_LIBRARY(matador-object SHARED ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-object matador-utils)

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-object
  PROPERTIES
  VERSION 0.5.0
  SOVERSION 0)

SOURCE_GROUP("include\\matador\\object" FILES ${HEADER})
SOURCE_GROUP("src\\matador\\object" FILES ${SOURCES})

INSTALL(
  TARGETS matador-object
  ARCHIVE  DESTINATION lib
  LIBRARY DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  TARGETS matador-object
  DESTINATION lib
  COMPONENT libraries
)

INSTALL(
  FILES ${HEADER}
  DESTINATION include/matador/object
  COMPONENT headers
)
