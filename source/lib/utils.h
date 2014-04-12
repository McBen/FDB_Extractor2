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

bool IsValidUTF8Char(const uint8_t* run,int max_chars);
bool IsValidUTF8String(const uint8_t* run,int len);
void MakeValidUTF8String(std::string& str);

#ifndef WIN32
    int _getch();
#else
    #include <conio.h>
#endif

#endif
