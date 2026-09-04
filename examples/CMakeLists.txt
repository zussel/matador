add_executable(schema schema.cpp)

target_link_libraries(schema
    PRIVATE matador
)
