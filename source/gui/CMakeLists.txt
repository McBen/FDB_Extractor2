set (HEADER_FILES
    gui_def.h
    main.h
	stdafx.h
    MainWindow.h
    Export_DLG.h
	DirectoryListCtrl.h
    FileListCtrl.h
    FDB_Collection.h
  )

set (SOURCE_FILES
    gui_def.cpp
    main.cpp
    MainWindow.cpp
    Export_DLG.cpp
	DirectoryListCtrl.cpp
    FileListCtrl.cpp
    FDB_Collection.cpp
  )

IF (WIN32)
    set (SOURCE_RESOURCES
        resource.h
        FDB_Extractor2.rc
    )
ENDIF()

INCLUDE_DIRECTORIES(
    ${FDB_LIB_INCLUDE}
    ${wxWidgets_INCLUDE_DIRS}
    )

IF(wxWidgets_FOUND)
    INCLUDE(${wxWidgets_USE_FILE})
ENDIF()

LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

ADD_EXECUTABLE(FDB_Extractor2 WIN32
    ${HEADER_FILES}
    ${SOURCE_FILES}
    ${SOURCE_RESOURCES})

SET_OUT_DIR(FDB_Extractor2)

ADD_DEPENDENCIES( FDB_Extractor2 FDB_LIB )
TARGET_LINK_LIBRARIES( FDB_Extractor2 FDB_LIB ${wxWidgets_LIBRARIES})

configure_file(${FDB_ex2_SOURCE_DIR}/resources/filetype_icons.png ${EXECUTABLE_OUTPUT_PATH}/img/filetype_icons.png COPYONLY)