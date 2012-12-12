set (HEADER_FILES
    commandline.h
  )

set (SOURCE_FILES
    main.cpp
    commandline.cpp
  )

INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
    ${FDB_LIB_INCLUDE}
    )

LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

SOURCE_GROUP("" FILES "../VersionNo.h")

ADD_EXECUTABLE(FDB_ex2
    ${HEADER_FILES}
    ${SOURCE_FILES}
    "../VersionNo.h"
    )

SET_OUT_DIR(FDB_ex2)


ADD_DEPENDENCIES( FDB_ex2 FDB_LIB)
TARGET_LINK_LIBRARIES( FDB_ex2 FDB_LIB ${Boost_LIBRARIES})


