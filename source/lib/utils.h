#include <string>

#ifndef UTILS_H
#define UTILS_H

namespace std
{
    string ReplaceString(string input, const string &search, const string &replace);
	string EscapeCSV_String(const string&);
	string EscapeSQLITE3_String(const string&);
	string EscapeMySQL_String(const string&, bool path_convert=true);

	string GetROMInstallDir();
}

#ifndef WIN32
    int _getch();
#else
    #include <conio.h>
#endif

#endif
