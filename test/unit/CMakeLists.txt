SET (TEST_SOURCES test_unit.cpp)

ADD_EXECUTABLE(test_unit ${TEST_SOURCES})

TARGET_LINK_LIBRARIES(test_unit oos)
