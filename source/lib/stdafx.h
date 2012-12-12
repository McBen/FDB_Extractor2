#include <Windows.h>

// 3rd party
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include <string>
#include <iostream>
#include <iomanip>


#if _MSC_VER < 1400
int fopen_s(FILE** f, const char*na, const char* at);
#endif

// our
#include "FDB_LIB.h"


