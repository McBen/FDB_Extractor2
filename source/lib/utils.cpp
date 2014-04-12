#include "stdafx.h"

#include "utils.h"
#include <sstream>

#if WIN32
  #include <windows.h>
#endif

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

	string EscapeMySQL_String(const string& instr, bool path_convert)
	{
	    string str=instr;
	
		str = ReplaceString(str, "\n", "\\n");
		str = ReplaceString(str, "'", "''");

		if (path_convert) {
			str = ReplaceString(str, "\\", "/");
		} else {
            str = ReplaceString(str, "\\", "\\\\");
        }


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
}

bool IsValidUTF8Char(const uint8_t* run,int max_chars)
{
    // U+0000..U+007F     00..7F
    if (*run < 0x20 && *run!=10 && *run!=0 && *run!=13) return false;
    if (*run < 0x80) return true;
    if (*run > 0xf4) return false;

    // U+0080..U+07FF     C2..DF     80..BF
    if (max_chars<2) return false;
    if (*run>=0xc2 && *run<=0xdf)   return (run[1]>=0x80 && run[1]<=0xbf);

    //  U+0800..U+0FFF     E0         A0..BF      80..BF
    if (max_chars<3) return false;
    if (*run==0xe0)   return (run[1]>=0xa0 && run[1]<=0xbf) && (run[2]>=0x80 && run[2]<=0xbf);
    //  U+1000..U+CFFF     E1..EC     80..BF      80..BF
    if (*run>=0xe1 && *run<=0xec)   return (run[1]>=0x80 && run[1]<=0xbf) && (run[2]>=0x80 && run[2]<=0xbf);
    //  U+D000..U+D7FF     ED         80..9F      80..BF
    if (*run==0xed)   return (run[1]>=0x80 && run[1]<=0x9f) && (run[2]>=0x80 && run[2]<=0xbf);
    //   U+E000..U+FFFF     EE..EF     80..BF      80..BF
    if (*run>=0xee && *run<=0xef)   return (run[1]>=0x80 && run[1]<=0xbf) && (run[2]>=0x80 && run[2]<=0xbf);

    //  U+10000..U+3FFFF   F0         90..BF      80..BF     80..BF
    if (max_chars<4) return false;
    if (*run==0xf0)   return (run[1]>=0x90 && run[1]<=0xbf) && (run[2]>=0x80 && run[2]<=0xbf)  && (run[3]>=0x80 && run[3]<=0xbf);
    // U+40000..U+FFFFF   F1..F3     80..BF      80..BF     80..BF
    if (*run>=0xf1 && *run<=0xf3)   return (run[1]>=0x80 && run[1]<=0xbf) && (run[2]>=0x80 && run[2]<=0xbf)  && (run[3]>=0x80 && run[3]<=0xbf);
    // U+100000..U+10FFFF F4         80..8F      80..BF     80..BF
    if (*run==0xf4)   return (run[1]>=0x80 && run[1]<=0x8f) && (run[2]>=0x80 && run[2]<=0xbf) && (run[3]>=0x80 && run[3]<=0xbf);

    return false;
}

size_t UTF8CharLen(const uint8_t* run)
{
    // 1 CHAR
    // U+0000..U+007F     00..7F
    if (*run < 0x80) return 1;
    if (*run > 0xf4) return 0;

    // 2 CHAR
    // U+0080..U+07FF     C2..DF     80..BF
    if (*run>=0xc2 && *run<=0xdf)   return (run[1]>=0x80 && run[1]<=0xbf) ? 2:0;

    // 3 CHAR
    //  U+0800..U+0FFF     E0         A0..BF      80..BF
    if (*run==0xe0)   return ((run[1]>=0xa0 && run[1]<=0xbf) && (run[2]>=0x80 && run[2]<=0xbf)) ? 3:0;
    //  U+1000..U+CFFF     E1..EC     80..BF      80..BF
    if (*run>=0xe1 && *run<=0xec)   return ((run[1]>=0x80 && run[1]<=0xbf) && (run[2]>=0x80 && run[2]<=0xbf)) ? 3:0;
    //  U+D000..U+D7FF     ED         80..9F      80..BF
    if (*run==0xed)   return ((run[1]>=0x80 && run[1]<=0x9f) && (run[2]>=0x80 && run[2]<=0xbf)) ? 3:0;
    //   U+E000..U+FFFF     EE..EF     80..BF      80..BF
    if (*run>=0xee && *run<=0xef)   return ((run[1]>=0x80 && run[1]<=0xbf) && (run[2]>=0x80 && run[2]<=0xbf)) ? 3:0;

    //  U+10000..U+3FFFF   F0         90..BF      80..BF     80..BF
    if (*run==0xf0)   return ((run[1]>=0x90 && run[1]<=0xbf) && (run[2]>=0x80 && run[2]<=0xbf)  && (run[3]>=0x80 && run[3]<=0xbf)) ? 4:0;
    // U+40000..U+FFFFF   F1..F3     80..BF      80..BF     80..BF
    if (*run>=0xf1 && *run<=0xf3)   return ((run[1]>=0x80 && run[1]<=0xbf) && (run[2]>=0x80 && run[2]<=0xbf)  && (run[3]>=0x80 && run[3]<=0xbf)) ? 4:0;
    // U+100000..U+10FFFF F4         80..8F      80..BF     80..BF
    if (*run==0xf4)   return ((run[1]>=0x80 && run[1]<=0x8f) && (run[2]>=0x80 && run[2]<=0xbf) && (run[3]>=0x80 && run[3]<=0xbf)) ? 4:0;

    return 0;
}

bool IsValidUTF8String(const uint8_t* run,int len)
{
    while (len>0) {
        int l = UTF8CharLen(run);
        assert(IsValidUTF8Char(run,len)==(l>0 && l<=len)); // test of UTF8CharLen function
        if (l==0) return false;
        len -= l;
        run += l;
    }

    return len==0;
}

void MakeValidUTF8String(std::string& str)
{
    for (size_t i = 0; i < str.size(); ) {
        size_t l = UTF8CharLen((uint8_t*)&str[i]);
        if (l==0 || l>(str.size()-i))  {
            str[i] = 0x7f;
            ++i;
        } else {
            i += l;
        }
    }
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



