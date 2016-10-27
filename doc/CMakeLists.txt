FIND_PACKAGE(Doxygen)

IF(DOXYGEN_FOUND)
    MESSAGE(STATUS "Doxygen detected - building documentation target")
    # Configure the Template Doxyfile for our specific project
    configure_file(doxygen/oos-api.dxy.in ${PROJECT_BINARY_DIR}/oos-api.dxy @ONLY IMMEDIATE)
    configure_file(doxygen/oos.dxy.in ${PROJECT_BINARY_DIR}/oos.dxy @ONLY IMMEDIATE)
    # Add a custom target to run Doxygen when ever the project is built
    add_custom_target (doc
                       COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/oos-api.dxy
                       COMMENT "Generating API documentation with Doxygen" VERBATIM)
    add_custom_target (web
                       COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/oos.dxy
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/doc/web ${CMAKE_BINARY_DIR}/web
                       COMMENT "Generating WEB documentation with Doxygen" VERBATIM)

    # Todo: replace links for jekyll
    # sed -i 's/href="\([^"#]*\).html\(#[^"]*\)\?"/href="\/api\/\1\/index.html\2"/g' *.html
ELSE(DOXYGEN_FOUND)
    MESSAGE(STATUS "Doxygen not detected - no documentation target")
ENDIF(DOXYGEN_FOUND)
