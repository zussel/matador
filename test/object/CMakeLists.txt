SET (TEST_SOURCES test_object.cpp Album.cpp Artist.cpp Track.cpp ComplexObjectStoreTestUnit.cpp)

ADD_EXECUTABLE(test_object ${TEST_SOURCES})

TARGET_LINK_LIBRARIES(test_object oos)
