SET(OBJECT_SOURCES object/object.cpp object/object_atomizer.cpp object/object_ptr.cpp object/object_store.cpp object/object_list.cpp object/object_proxy.cpp object/object_serializer.cpp object/prototype_node.cpp)
SET(TOOLS_SOURCES tools/byte_buffer.cpp)
SET(UNIT_SOURCES unit/test_suite.cpp unit/unit_exception.cpp unit/unit_test.cpp)

ADD_LIBRARY(oos STATIC ${OBJECT_SOURCES} ${TOOLS_SOURCES} ${UNIT_SOURCES})

