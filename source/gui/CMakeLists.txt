set (HEADER_FILES
    gui_def.h
    main.h
	stdafx.h
    MainWindow.h
    Export_DLG.h
	DirectoryListCtrl.h
    FileListCtrl.h
    FDB_Collection.h
    ../../resources/images/wxresource.h
  )

set (SOURCE_FILES
    gui_def.cpp
    main.cpp
    MainWindow.cpp
    Export_DLG.cpp
	DirectoryListCtrl.cpp
    FileListCtrl.cpp
    FDB_Collection.cpp
    ../../resources/images/wxresource.cpp
  )

IF (WIN32)
    set (SOURCE_RESOURCES
        resource.h
        FDB_Extractor2.rc
    )
ENDIF()

SOURCE_GROUP("" FILES "../VersionNo.h")

INCLUDE_DIRECTORIES(${FDB_LIB_INCLUDE})


IF(wxWidgets_FOUND)

    INCLUDE_DIRECTORIES(
        ${wxWidgets_LIB_DIR}
        ${wxWidgets_INCLUDE_DIRS}
        )

    # cmake --help-policy CMP0043
    INCLUDE(${wxWidgets_USE_FILE})

    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS} ${wxWidgets_LIB_DIR})

    ADD_EXECUTABLE(FDB_Extractor2 WIN32
        ${HEADER_FILES}
        ${SOURCE_FILES}
        ${SOURCE_RESOURCES}
        "../VersionNo.h"
        )

    TARGET_LINK_LIBRARIES( FDB_Extractor2 ${wxWidgets_LIBRARIES})

ELSE()

    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

    ADD_EXECUTABLE(FDB_Extractor2 WIN32 EXCLUDE_FROM_ALL
        ${HEADER_FILES}
        ${SOURCE_FILES}
        ${SOURCE_RESOURCES}
        "../VersionNo.h"
        )

    set_target_properties(FDB_Extractor2 PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)

ENDIF()

SET_OUT_DIR(FDB_Extractor2)
ADD_DEPENDENCIES( FDB_Extractor2 FDB_LIB )
TARGET_LINK_LIBRARIES( FDB_Extractor2 FDB_LIB)
