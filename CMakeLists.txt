######################################
# CMAKE settings / workarounds
######################################
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)


# Force out-of-source build
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" BUILDING_IN_SOURCE)
if(BUILDING_IN_SOURCE)
  message(FATAL_ERROR
    "This project requires an out of source build. Remove the file 'CMakeCache.txt' found in this directory before continuing, create a separate build directory and run 'cmake <srcs> [options]' from there."
  )
endif()

# Remove /STACK:10000000 set by CMake. This value for stack size
# is very high, limiting the number of threads we can spawn.
# Default value used by Windows is 1MB which is good enough.
STRING(REGEX REPLACE "/STACK:[0-9]+" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
STRING(REGEX REPLACE "/STACK:[0-9]+" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")


#set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "")
SET(BUILD_SHARED_LIBS FALSE)
SET(CMAKE_DEBUG_POSTFIX "d")

#set(CMAKE_LEGACY_CYGWIN_WIN32 1)

######################################
# Set up the basic build environment
######################################
OPTION(FDB_INCLUDE_TESTS "Include test-classes" true)

PROJECT(FDB_ex2)


SET(EXECUTABLE_OUTPUT_PATH "${FDB_ex2_SOURCE_DIR}/bin")

macro(SET_OUT_DIR target) # to remove /debug /release sub dirs on MS
  SET_TARGET_PROPERTIES( ${target} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_PATH}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXECUTABLE_OUTPUT_PATH}"
    )
endmacro()



######################################
# Dependecies
######################################

# ZLIB
set(SKIP_INSTALL_ALL)
add_subdirectory(dep/zlib-1.2.5)
set(ZLIB_INCLUDE "${FDB_ex2_SOURCE_DIR}/dep/zlib-1.2.5" "${CMAKE_CURRENT_BINARY_DIR}/dep/zlib-1.2.5")

# WXWIDGET
find_package( wxWidgets COMPONENTS core base)

# BOOST
#set(Boost_DEBUG ON) # make sure to build: "b2 link=static threading=multi runtime-link=static"
set(Boost_USE_STATIC_LIBS       ON) # "lib..."
set(Boost_USE_MULTITHREADED     ON) # "-mt"
#set(Boost_USE_STATIC_RUNTIME    ON) # "-s"
find_package( Boost 1.47.0 REQUIRED COMPONENTS program_options regex filesystem system)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "ERROR: boost not found.\nmake sure you build with: 'b2 link=static threading=multi runtime-link=static'")
endif()

# LZO
add_subdirectory(dep/lzo-2.06)

######################################
# RoM Files
######################################
set(redux1_dll  ${FDB_ex2_SOURCE_DIR}/dep/RoM/redux_runtime.dll)
set(redux2_dll  ${FDB_ex2_SOURCE_DIR}/dep/RoM/redux_nvtt.dll)
IF (NOT EXISTS "${redux_dll}")
    find_path(FDB_ROM_Path NAMES "redux_runtime.dll"
        PATHS "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Frogster Interactive Pictures\\Runes of Magic;RootDir]"
             "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Frogster Interactive Pictures\\Runes of Magic;RootDir]"
             "$ENV{ProgramFiles}/Runes of Magic"
        DOC "Runes of Magic directory"
        )

    if (FDB_ROM_Path AND EXISTS "${FDB_ROM_Path}/redux_runtime.dll")
        configure_file("${FDB_ROM_Path}/redux_runtime.dll"  "${redux1_dll}" COPYONLY)
        configure_file("${FDB_ROM_Path}/redux_nvtt.dll"  "${redux2_dll}" COPYONLY)
    else()
        message(WARNING "redux_runtime.dll not found (copyright issue)\n-manually copy it or set-up RoM-Directory")
    endif()
endif()


#configure_file(${FDB_ex2_SOURCE_DIR}/dep/RoM/redux_runtime.dll ${EXECUTABLE_OUTPUT_PATH}/redux_runtime.dll COPYONLY)
#configure_file(${FDB_ex2_SOURCE_DIR}/dep/RoM/redux_nvtt.dll ${EXECUTABLE_OUTPUT_PATH}/redux_nvtt.dll COPYONLY)

configure_file(${FDB_ex2_SOURCE_DIR}/field_def/db_crc.csv ${EXECUTABLE_OUTPUT_PATH}/db_crc.csv COPYONLY)
configure_file(${FDB_ex2_SOURCE_DIR}/field_def/fields.csv ${EXECUTABLE_OUTPUT_PATH}/fields.csv COPYONLY)



######################################
# Projects
######################################
add_subdirectory(source)

set_target_properties(zlib PROPERTIES FOLDER "3rd_party")
IF (NOT wxWidgets_FOUND)
    set_target_properties(FDB_Extractor2 PROPERTIES EXCLUDE_FROM_ALL true)
ENDIF()