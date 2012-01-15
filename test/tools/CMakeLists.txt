SET (TEST_SOURCES
  test_tools.cpp
  BlobTestUnit.hpp
  BlobTestUnit.cpp
)

ADD_EXECUTABLE(test_tools ${TEST_SOURCES})

TARGET_LINK_LIBRARIES(test_tools oos ${CMAKE_DL_LIBS})
