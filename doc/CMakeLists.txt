find_package(Doxygen REQUIRED)

if(DOXYGEN_FOUND)
    message(STATUS "Doxygen detected - building documentation target")

    include(FetchContent)
    FetchContent_Declare(
            doxygen-awesome-css
            URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
    )
    FetchContent_MakeAvailable(doxygen-awesome-css)

    # Save the location the files were cloned into
    # This allows us to get the path to doxygen-awesome.css
    FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

    # Generate the Doxyfile
    set(DOXYGEN_IN_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in")
    set(DOXYGEN_OUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

    # Doxyfile.in einlesen und Platzhalter (wie @AWESOME_CSS_DIR@) ersetzen
    configure_file(${DOXYGEN_IN_FILE} ${DOXYGEN_OUT_FILE} @ONLY)

    add_custom_target(doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT_FILE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generiere HTML-Dokumentation mit Doxygen Awesome Theme..."
            VERBATIM
    )
else(DOXYGEN_FOUND)
    message(STATUS "Doxygen not detected - no documentation target")
endif(DOXYGEN_FOUND)
