#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### Version 1.3
#### DO NOT CHANGE!
######################################################################


## CREATE INCLUDE DIRECTORY
HEADERS_DIR_PATH = $${PWD}/include
mkpath($${HEADERS_DIR_PATH})

# custom copy command
MY_COPY = $$QMAKE_COPY

# QMAKE_COPY may not work reliable on win
win32: {
    MY_COPY = "xcopy /y "
}

## FUNCTION DEFINITION FOR COPY FUNCTION
defineTest(copyHeaders) {

    files = $$1
    keepIncludePaths = $$2
    privateIncludePaths = $$3

    includeDir = $${HEADERS_DIR_PATH}/$${TARGET_DIR_NAME}

    win32 {

        includeDir ~= s,/,\\,g
    }

    headerDirs =

    for(file, files) {

        dirName = $$dirname(file)

        !isEmpty(dirName) {

            exists($$file) {

                !contains(headerDirs, $$dirName) {

                    !contains(privateIncludePaths, $$dirName) {

                        headerDirs += $$dirName
                    }
                }
            }
        }
    }

    win32 {

        QMAKE_POST_LINK += if not exist $$shell_quote($$includeDir) $$QMAKE_MKDIR $$shell_quote($$includeDir) $$escape_expand(\\n\\t)

        exists(*.h) {
            QMAKE_POST_LINK += $$MY_COPY $$shell_quote(*.h) $$shell_quote($$includeDir) $$escape_expand(\\n\\t)
        }

    }
    unix {

        QMAKE_POST_LINK += $$QMAKE_CHK_DIR_EXISTS $$shell_quote($$includeDir) || $$QMAKE_MKDIR $$shell_quote($$includeDir) $$escape_expand(\\n\\t)

        exists(*.h) {
            QMAKE_POST_LINK += find . -maxdepth 1 -name $$shell_quote(*.h) -exec cp $$shell_quote({}) $$shell_quote($$includeDir) \; $$escape_expand(\\n\\t)
        }
    }


    for(dirName, headerDirs) {

        copyTo = $$includeDir

        win32 {

            copyTo ~= s,/,\\,g
        }

        contains(keepIncludePaths, $$dirName) {

            copyTo = $$copyTo/$$dirName

            win32 {

                copyTo ~= s,/,\\,g
                QMAKE_POST_LINK += if not exist $$shell_quote($$copyTo) $$QMAKE_MKDIR $$shell_quote($$copyTo) $$escape_expand(\\n\\t)
            }
            unix {

                QMAKE_POST_LINK += $$QMAKE_CHK_DIR_EXISTS $$shell_quote($${copyTo}) || $$QMAKE_MKDIR $$shell_quote($${copyTo}) $$escape_expand(\\n\\t)
            }
        }        

        win32 {

            headersPattern = $${PWD}/$${dirName}/*.hpp
            headersPattern ~= s,/,\\,g

            exists($${headersPattern}) {

                QMAKE_POST_LINK += $$MY_COPY $$shell_quote($${headersPattern}) $$shell_quote($$copyTo) $$escape_expand(\\n\\t)
            }

            headersPattern = $${PWD}/$${dirName}/*.h
            headersPattern ~= s,/,\\,g

            exists($${headersPattern}) {

                QMAKE_POST_LINK += $$MY_COPY $$shell_quote($${headersPattern}) $$shell_quote($$copyTo) $$escape_expand(\\n\\t)
            }
        }
        unix {

            QMAKE_POST_LINK += find ./$${dirName} -maxdepth 1 -name $$shell_quote(*.h) -exec cp $$shell_quote({}) $$shell_quote($$copyTo) \; $$escape_expand(\\n\\t)
            QMAKE_POST_LINK += find ./$${dirName} -maxdepth 1 -name $$shell_quote(*.hpp) -exec cp $$shell_quote({}) $$shell_quote($$copyTo) \; $$escape_expand(\\n\\t)

        }
    }

    export(QMAKE_POST_LINK)

    return(true)
}

defineTest(copyToEnvironmentPathModules) {

    isEmpty(GTLAB_ENVIRONMENT_PATH) {

        return(false)
    }

    environmentPath = $${GTLAB_ENVIRONMENT_PATH}/modules

    copyToEnvironmentPath($$environmentPath)
}

defineTest(copyToEnvironmentPath) {

    isEmpty(GTLAB_ENVIRONMENT_PATH) {

        return(false)
    }

    isEmpty(DESTDIR) {

        return(false)
    }

    args = $$ARGS

    count(args, 1) {

        environmentPath = $$1
    } else {

        environmentPath = $${GTLAB_ENVIRONMENT_PATH}
    }

    win32:environmentPath ~= s,/,\\,g

    exists($$environmentPath) {

        win32 {
            dllPath = $${DESTDIR}/$${TARGET}.dll
            dllPath ~= s,/,\\,g

            QMAKE_POST_LINK += $$MY_COPY $$shell_quote($$dllPath) $$shell_quote($$environmentPath) $$escape_expand(\\n\\t)

            libPath = $${DESTDIR}/$${TARGET}.lib
            libPath ~= s,/,\\,g

            QMAKE_POST_LINK += $$MY_COPY $$shell_quote($$libPath) $$shell_quote($$environmentPath) $$escape_expand(\\n\\t)

        }

        unix:  QMAKE_POST_LINK += find $${DESTDIR} -name $$shell_quote(*$${TARGET}.so*) -exec cp $$shell_quote({}) $$shell_quote($$environmentPath) \; $$escape_expand(\\n\\t)

        export(QMAKE_POST_LINK)

        return(true)
    } else {

        warning(GTLAB_ENVIRONMENT_PATH ($${environmentPath}) does not exist!)
    }

    return(true)
}
######################################################################
