#include "stdafx.h"

#include "FDBFileStringDB.h"
#include "utils.h"
#include <ctype.h>
using namespace std;



FDBFileStringDB::FDBFileStringDB(const FDBPackage::file_info& s_info, uint8_t* data )
    : FDBFile(s_info,data)
{
}

bool FDBFileStringDB::WriteINI(const char* filename)
{
    FILE* outf;
    if (fopen_s(&outf, filename, "wb")) return false;
	if (!outf) return false;

    const char* run = (const char*)data;
    const char* end = (const char*)data+data_size;
    while (run<end)
    {

    	const s_entry* ent = (const s_entry*)run;
        run+= 64+4+ent->v_len;

        assert(strlen(ent->value)+1 == ent->v_len);

        std::string value = ReplaceString(std::string(ent->value),"\n","\\n");
        fprintf(outf,"\"%s\"=\"%s\"\n",ent->key,value.c_str());
    }

    fclose(outf);

    return true;
}

bool FDBFileStringDB::WriteSQLITE3(const char* filename)
{
	FILE* outf;
	if (fopen_s(&outf, filename, "wt")) return false;
	if (!outf) return false;

	// Options
	boost::filesystem::path filepath(filename);
	fprintf(outf, "-- %s\n", filepath.filename().generic_string().c_str());
	fprintf(outf, "\n");
	
	// Head
	string lang = filepath.stem().generic_string();

    fprintf(outf, "BEGIN TRANSACTION;\n");
    
	fprintf(outf, "CREATE TABLE IF NOT EXISTS `strings` (`guid` TEXT PRIMARY KEY);\n");

	fprintf(outf, "ALTER TABLE `strings` ADD COLUMN `%s` TEXT;\n\n", lang.c_str());

	const char* run = (const char*)data;
    const char* end = (const char*)data+data_size;
    while (run<end)
    {
        s_entry* ent = (s_entry*)run;
        run+= 64+4+ent->v_len;
        assert(strlen(ent->value)+1 == ent->v_len);

		if (strlen(ent->value)>0) 
			fprintf(outf,"INSERT OR IGNORE INTO `strings` (`guid`) VALUES ('%s');\n",EscapeSQLITE3_String(ent->key).c_str());
    }

	run = (const char*)data;
    while (run<end)
    {
        s_entry* ent = (s_entry*)run;
        run+= 64+4+ent->v_len;

        std::string value = ReplaceString(std::string(ent->value),"\n","\\n");
		if (!value.empty()) 
			fprintf(outf,"UPDATE `strings` SET `%s`='%s' where `guid`='%s';\n",lang.c_str(), EscapeSQLITE3_String(value).c_str(), EscapeSQLITE3_String(ent->key).c_str());
    }

	fprintf(outf, "\nEND TRANSACTION;\n");

	fclose(outf);
	return true;
}

