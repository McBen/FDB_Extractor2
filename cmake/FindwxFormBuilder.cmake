# - This module looks for wxFormBuilder
# wxFormBuilder is a GUI-Designer for wxWidget library. Please see
# http://wxformbuilder.org
#
# This modules defines the following variables:
#
#   wxFORMBUILDER_EXECUTABLE     = The path to the wxFormbuilder command.
#   WXFORMBUILDER_FOUND          = Was wxFormbuilder found or not?

MESSAGE(STATUS ${wxFB_PRJ_BINARY_DIR})
FIND_PROGRAM(wxFORMBUILDER_EXECUTABLE
  NAMES wxformbuilder wxFormBuilder
  PATHS
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\wxformbuilder;Inno Setup: App Path]/bin"
  DOC "wxWidgets Designer, GUI Builder, and RAD Tool (http://wxformbuilder.org)"
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(wxFormBuilder DEFAULT_MSG wxFORMBUILDER_EXECUTABLE)

MARK_AS_ADVANCED(
  WXFORMBUILDER_EXECUTABLE
  )

####################
# Use it to generate source
macro(WXFB_GENERATE output input)
  IF (NOT WXFORMBUILDER_FOUND)
    MESSAGE(ERROR "wxFormBuild not found")
  ENDIF()

  get_filename_component(outputname ${output} NAME_WE)
  get_filename_component(outputpath ${output} PATH)
  IF(IS_DIRECTORY ${outputpath})
    SET(outputname "${outputpath}/${outputname}")
  ENDIF()

  add_custom_command( OUTPUT "${outputname}.cpp" "${outputname}.h"
                      COMMAND ${wxFORMBUILDER_EXECUTABLE} "-g" "${input}"
                      DEPENDS ${input}
                    )
endmacro()