#include "stdafx.h"

#include "utils.h"
#include <sstream>

<<<<<<< HEAD
#if WIN32
  #include <windows.h>
#endif
=======
#include <windows.h>
>>>>>>> some mingw compatibilies

#if _MSC_VER < 1400
int fopen_s(FILE** f, const char*na, const char* at)
{
    *f = fopen(na,at);
    if (*f==NULL) return 1;
    return 0;
}
#endif


namespace std
{

    string ReplaceString(string input, const string &search, const string &replace )
    {
        string::size_type pos = input.find(search, 0);
        int LengthSearch = search.length();
        int LengthReplacment = replace.length();

        while(string::npos != pos)
        {
            input.replace(pos, LengthSearch, replace);
            pos = input.find(search, pos + LengthReplacment);
        }

        return input;
    }

	string EscapeCSV_String(const string&instr)
	{
		string str=instr;
    
		bool quotes_needed = (str.find_first_of(";\"\n\r") != string::npos);

		str = ReplaceString(str, "\n", "\\n");
		str = ReplaceString(str, "\r", "\\r");
		str = ReplaceString(str, "\"", "\"\"");

	    if (quotes_needed)  str = string("\"") + str + string("\"");

		return str;
	}

	string EscapeSQLITE3_String(const string& instr)
	{
	    string str=instr;
		str = ReplaceString(str, "\n", "\\n");
		str = ReplaceString(str, "'", "''");

		return str;
	}

	string GetROMInstallDir()
	{
		#if WIN32
			HKEY hKey;
			LONG lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Frogster Interactive Pictures\\Runes of Magic", 0, KEY_READ, &hKey);
			if (lRes != ERROR_SUCCESS) return "";

			CHAR szBuffer[512];
			DWORD dwBufferSize = sizeof(szBuffer);
			ULONG nError = RegQueryValueEx(hKey, "RootDir", 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
			if (ERROR_SUCCESS == nError)
			{
				return szBuffer;
			}
		#endif

		return "";
	}

	//bool GetFileVersion(const string& filename, DWORD& hi,DWORD& lo)
	//{
	//    try
	//	{
	//		DWORD size, dummy;
	//		size = GetFileVersionInfoSize(filename.c_str(), &dummy);
	//		if (size)
	//		{
	//			char *buf = (char *)malloc(size);
	//			GetFileVersionInfo(filename.c_str(), NULL, size, buf);
	//			VS_FIXEDFILEINFO *qbuf;
	//			UINT len;
	//			if (VerQueryValue(buf, "\\", (void **)&qbuf, &len)) 
	//			{
	//				hi = qbuf->dwFileVersionMS;
	//				lo = qbuf->dwFileVersionLS;
	//				free(buf);
	//				return true;
	//			}
	//			free(buf);
	//		}
	//    } catch(...)	{};

	//	return false;
	//}


	//string GetFileVersionString(const string& filename)
	//{
	//	DWORD hi, lo;
	//	if (!GetFileVersion(filename, hi,lo)) return "";

	//	ostringstream out;
	//	if (LOWORD(lo))      out << HIWORD(hi) <<"."<< LOWORD(hi) <<"."<< HIWORD(lo) <<"."<< LOWORD(lo);
	//	else if (HIWORD(lo)) out << HIWORD(hi) <<"."<< LOWORD(hi) <<"."<< HIWORD(lo);
	//	else                 out << HIWORD(hi) <<"."<< LOWORD(hi);

	//	return out.str();
	//}
}



#ifndef WIN32
    #include <termios.h>

    int _getch()
    {
        static int ch = -1, fd = 0;
        struct termios neu, alt;
        fd = fileno(stdin);
        tcgetattr(fd, &alt);
        neu = alt;
        neu.c_lflag &= ~(ICANON|ECHO);
        tcsetattr(fd, TCSANOW, &neu);
        ch = getchar();
        tcsetattr(fd, TCSANOW, &alt);
        return ch;
     }
#endif



