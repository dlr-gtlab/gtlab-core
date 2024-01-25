option (GTLAB_BUILD_DOCS "Build gtlab developer documentation (requires doxygen and sphinx)" OFF)

if (GTLAB_BUILD_DOCS)

    find_package(Doxygen REQUIRED)
    find_package(Sphinx REQUIRED)

    #This will be the main output of our command
    set(DOXYGEN_INDEX_FILE ${CMAKE_CURRENT_SOURCE_DIR}/html/index.html)

    set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/src)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doxygen)
    set(DOXYFILE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(SPHINXCONF ${CMAKE_CURRENT_SOURCE_DIR}/conf.py)



    file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) #Doxygen won't create this for us
    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/xml/index.xml
                       COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
                       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                       MAIN_DEPENDENCY ${DOXYFILE} ${DOXYFILE_IN}
                       COMMENT "Generating docs")

    add_custom_target(Doxygen DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/xml/index.xml)

    set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
    set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/html)

    add_custom_target(doc
                      DEPENDS Doxygen
                      COMMAND
                      ${SPHINX_EXECUTABLE} -b html
                      ${SPHINX_SOURCE} ${SPHINX_BUILD}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generating documentation with Sphinx")

    install(DIRECTORY ${SPHINX_BUILD} DESTINATION ${CMAKE_INSTALL_DOCDIR})

endif(GTLAB_BUILD_DOCS)
