INCLUDE_DIRECTORIES(${OOS_SOURCE_DIR}/../include)

ADD_EXECUTABLE(test_object test_object Album Artist Track)

TARGET_LINK_LIBRARIES(test_object oos)
