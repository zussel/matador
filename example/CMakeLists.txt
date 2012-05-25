SET (EXAMPLE_SOURCES
  Command.hpp
  Create.hpp
  Create.cpp
  Clear.hpp
  Clear.cpp
  Import.hpp
  Import.cpp
  List.hpp
  List.cpp
  TablePrinter.hpp
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
