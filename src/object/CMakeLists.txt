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
  object_proxy_accessor.cpp
  object_deleter.cpp
  object_inserter.cpp
  relation_field_endpoint.cpp
  abstract_has_many.cpp
  basic_has_many_to_many_item.cpp
  prototype_info.cpp json_object_mapper.cpp json_object_serializer.cpp object_json_serializer.cpp json_object_mapper_serializer.cpp)

SET(HEADER
  ../../include/matador/object/action.hpp
  ../../include/matador/object/attribute_serializer.hpp
  ../../include/matador/object/generic_access.hpp
  ../../include/matador/object/object_exception.hpp
  ../../include/matador/object/object_ptr.hpp
  ../../include/matador/object/object_ptr.tpp
  ../../include/matador/object/object_store.hpp
  ../../include/matador/object/object_view.hpp
  ../../include/matador/object/object_proxy.hpp
  ../../include/matador/object/object_serializer.hpp
  ../../include/matador/object/prototype_node.hpp
  ../../include/matador/object/typed_object_store_observer.hpp
  ../../include/matador/object/object_expression.hpp
  ../../include/matador/object/attribute_serializer.hpp
  ../../include/matador/object/prototype_iterator.hpp
  ../../include/matador/object/has_many.hpp
  ../../include/matador/object/has_one.hpp
  ../../include/matador/object/json_object_mapper.hpp
  ../../include/matador/object/container_type_traits.hpp
  ../../include/matador/object/object_holder.hpp
  ../../include/matador/object/basic_has_many.hpp
  ../../include/matador/object/has_many_vector.hpp
  ../../include/matador/object/has_many_list.hpp
  ../../include/matador/object/has_many_set.hpp
  ../../include/matador/object/transaction.hpp
  ../../include/matador/object/action_inserter.hpp
  ../../include/matador/object/action_visitor.hpp
  ../../include/matador/object/action_remover.hpp
  ../../include/matador/object/insert_action.hpp
  ../../include/matador/object/update_action.hpp
  ../../include/matador/object/delete_action.hpp
  ../../include/matador/object/abstract_has_many.hpp
  ../../include/matador/object/has_one_to_many_item.hpp
  ../../include/matador/object/basic_has_many_item_holder.hpp
  ../../include/matador/object/identifier_proxy_map.hpp
  ../../include/matador/object/object_proxy_accessor.hpp
  ../../include/matador/object/belongs_to.hpp
  ../../include/matador/object/object_holder_type.hpp
  ../../include/matador/object/object_inserter.hpp
  ../../include/matador/object/object_inserter.tpp
  ../../include/matador/object/object_deleter.hpp
  ../../include/matador/object/object_deleter.tpp
  ../../include/matador/object/node_analyzer.hpp
  ../../include/matador/object/node_analyzer.tpp
  ../../include/matador/object/relation_field_endpoint.hpp
  ../../include/matador/object/relation_field_endpoint.tpp
  ../../include/matador/object/has_many_to_many_item.hpp
  ../../include/matador/object/basic_has_many_to_many_item.hpp
  ../../include/matador/object/has_many_item_holder.hpp
  ../../include/matador/object/prototype_info.hpp
  ../../include/matador/object/relation_endpoint_value_inserter.hpp
  ../../include/matador/object/relation_endpoint_value_inserter.tpp
  ../../include/matador/object/relation_endpoint_value_remover.hpp
  ../../include/matador/object/relation_endpoint_value_remover.tpp
  ../../include/matador/object/to_many_endpoints.hpp
  ../../include/matador/object/has_many_iterator_traits.hpp ../../include/matador/object/json_object_serializer.hpp ../../include/matador/object/object_json_serializer.hpp ../../include/matador/object/json_object_mapper_serializer.hpp)

ADD_LIBRARY(matador-object SHARED ${SOURCES} ${HEADER})

TARGET_LINK_LIBRARIES(matador-object matador-utils)

# Set the build version (VERSION) and the API version (SOVERSION)
SET_TARGET_PROPERTIES(matador-object
  PROPERTIES
  VERSION ${APP_VERSION}
  SOVERSION ${APP_MAJOR_VERSION})

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
