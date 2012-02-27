SET (EXAMPLE_SOURCES
  Album.cpp
  Album.hpp
  Artist.cpp
  Artist.hpp
  Track.cpp
  Track.hpp
  main.cpp
)

ADD_EXECUTABLE(mdb ${EXAMPLE_SOURCES})

TARGET_LINK_LIBRARIES(mdb oos ${CMAKE_DL_LIBS})
