ADD_EXECUTABLE(rootless main.cpp person.hpp)

TARGET_LINK_LIBRARIES(rootless oos ${CMAKE_DL_LIBS})
