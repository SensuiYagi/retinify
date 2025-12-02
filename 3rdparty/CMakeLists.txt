add_subdirectory(json)

if(BUILD_TESTS)
    set(INSTALL_GTEST OFF CACHE BOOL "")
    add_subdirectory(googletest)
endif(BUILD_TESTS)
