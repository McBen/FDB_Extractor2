
add_subdirectory(lib)
add_subdirectory(cmd)
add_subdirectory(gui)


if (FDB_INCLUDE_TESTS)
    # if not found -> build debug+release in msvc
    set(GTEST_ROOT "${FDB_ex2_SOURCE_DIR}/dep/gtest-1.6.0")
    find_package(GTest REQUIRED)
    enable_testing(true)
    add_subdirectory(test)
endif()
