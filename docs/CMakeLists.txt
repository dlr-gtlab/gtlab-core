option (GTLAB_BUILD_DOCS "Build gtlab developer documentation (requires doxygen and sphinx)" OFF)

if (GTLAB_BUILD_DOCS)

    find_package(Doxygen REQUIRED)
    find_package(Sphinx REQUIRED)

    #This will be the main output of our command
    set(DOXYGEN_INDEX_FILE ${CMAKE_CURRENT_SOURCE_DIR}/html/index.html)

    set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/src)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    set(SPHINXCONF_IN ${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in)
    set(SPHINXCONF_OUT ${CMAKE_CURRENT_BINARY_DIR}/conf.py)

    #Replace variables inside @@ with the current values
    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)
    configure_file(${SPHINXCONF_IN} ${SPHINXCONF_OUT} @ONLY)

    file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) #Doxygen won't create this for us
    add_custom_command(OUTPUT ${DOXYGEN_OUTPUT_DIR}/xml/index.xml
                       COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                       MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                       COMMENT "Generating docs")

    add_custom_target(Doxygen DEPENDS ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)

    set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
    set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/html)

    add_custom_target(doc
                      DEPENDS Doxygen
                      COMMAND
                      ${SPHINX_EXECUTABLE} -b html -c ${CMAKE_CURRENT_BINARY_DIR}
                      ${SPHINX_SOURCE} ${SPHINX_BUILD}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generating documentation with Sphinx")

    install(DIRECTORY ${SPHINX_BUILD} DESTINATION ${CMAKE_INSTALL_DOCDIR})

endif(GTLAB_BUILD_DOCS)
