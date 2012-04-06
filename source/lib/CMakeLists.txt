set (HEADER_FILES
    FDB_LIB.h
    csv_parser.hpp
    utils.h
    decrypte.h
    FDBFile.h
    FDBFileDB.h
    FDBFileStringDB.h
    FDBField.h
    FDBFieldDef.h
    FDBFieldManager.h
    FDBFieldAnalyzer.h
    FDBFileImage.h
    FDBPackage.h
    mode4.h
    stdafx.h
  )

set (SOURCE_FILES
    csv_parser.cpp
    FDBFile.cpp
    FDBFileDB.cpp
    FDBFileStringDB.cpp
    FDBField.cpp
    FDBFieldDef.cpp
    FDBFieldManager.cpp
    FDBFieldAnalyzer.cpp
    FDBFileImage.cpp
    FDBPackage.cpp
    Mode1.cpp
    Mode2.cpp
    Mode3.cpp
    mode4.cpp
    utils.cpp
  )

INCLUDE_DIRECTORIES(
    ${ZLIB_INCLUDE}
    ${Boost_INCLUDE_DIRS}
    ${LZO_INCLUDE}
    )
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

 add_library(FDB_LIB
    ${HEADER_FILES}
    ${SOURCE_FILES} )

ADD_DEPENDENCIES( FDB_LIB zlib lzo_lib)
TARGET_LINK_LIBRARIES(FDB_LIB zlib lzo_lib)

SET(FDB_LIB_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
