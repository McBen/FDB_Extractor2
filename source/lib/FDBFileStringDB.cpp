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

        const s_entry* ent = reinterpret_cast<const s_entry*>(run);
        run+= 64+4+ent->v_len;
        assert(strlen(ent->value)+1 == ent->v_len);

        std::string value = ent->value;
        std::string key = ent->key;

        MakeValidUTF8String(value);
        MakeValidUTF8String(key);

        value = ReplaceString(value,"\n","\\n");
        value = ReplaceString(value ,"\t","\\t");
        if (value=="\xf")  value = ""; // special in string.db

        assert(IsValidUTF8String((uint8_t*) value.c_str(),value.size()));
        assert(IsValidUTF8String((uint8_t*) key.c_str(),key.size()));

        fprintf(outf,"\"%s\"=\"%s\"\n", key.c_str(), value.c_str());
    }

    fclose(outf);

    return true;
}

bool FDBFileStringDB::WriteSQLITE(const char* filename)
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
        const s_entry* ent = reinterpret_cast<const s_entry*>(run);
        run+= 64+4+ent->v_len;
        assert(strlen(ent->value)+1 == ent->v_len);

		if (strlen(ent->value)>0) 
			fprintf(outf,"INSERT OR IGNORE INTO `strings` (`guid`) VALUES ('%s');\n",EscapeSQLITE3_String(ent->key).c_str());
    }

	run = (const char*)data;
    while (run<end)
    {
        const s_entry* ent = reinterpret_cast<const s_entry*>(run);
        run+= 64+4+ent->v_len;

        std::string value = ReplaceString(std::string(ent->value),"\n","\\n");
		if (!value.empty()) 
			fprintf(outf,"UPDATE `strings` SET `%s`='%s' where `guid`='%s';\n",lang.c_str(), EscapeSQLITE3_String(value).c_str(), EscapeSQLITE3_String(ent->key).c_str());
    }

	fprintf(outf, "\nEND TRANSACTION;\n");

	fclose(outf);
	return true;
}

bool FDBFileStringDB::WriteMySQL(const char* filename)
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
    fprintf(outf, "/*!40101 SET NAMES utf8 */;\n\n");

	fprintf(outf, "CREATE TABLE IF NOT EXISTS `strings` (`guid` VARCHAR(50) PRIMARY KEY) DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;\n\n");

    fprintf(outf,   "delimiter '//'\n"
                    "CREATE PROCEDURE createcolumn() BEGIN\n"
                    "    DECLARE CONTINUE HANDLER FOR 1060 BEGIN END;\n"
                    "    ALTER TABLE `strings` ADD COLUMN `%s` TEXT;\n"
                    "END;//\n"
                    "delimiter ';'\n"
                    "CALL createcolumn();\n"
                    "DROP PROCEDURE createcolumn;\n\n", lang.c_str());

    
    fprintf(outf, "BEGIN;\n");


    int count=0;
	const char* run = (const char*)data;
    const char* end = (const char*)data+data_size;
    if (run<end) {
        fprintf(outf,"INSERT IGNORE INTO `strings` (`guid`) VALUES ");
        while (run<end)
        {
            const s_entry* ent = reinterpret_cast<const s_entry*>(run);
            run+= 64+4+ent->v_len;
            assert(strlen(ent->value)+1 == ent->v_len);

		    if (strlen(ent->value)>0) 
            {
                if (count>0) fprintf(outf,",");
			    fprintf(outf,"('%s')", EscapeMySQL_String(ent->key,false).c_str());
                count++;

                if (count>1000 && run<end) {
                    fprintf(outf,";\nINSERT IGNORE INTO `strings` (`guid`) VALUES ");
                    count=0;
                }
            }
        }

        fprintf(outf,";\n");
    }

	run = (const char*)data;
    while (run<end)
    {
        const s_entry* ent = reinterpret_cast<const s_entry*>(run);
        run+= 64+4+ent->v_len;

       std::string value = std::string(ent->value);
		if (!value.empty()) 
			fprintf(outf,"UPDATE `strings` SET `%s`='%s' where `guid`='%s';\n",lang.c_str(), EscapeMySQL_String(value,false).c_str(), EscapeMySQL_String(ent->key,false).c_str());
    }

	fprintf(outf, "\n");

    fprintf(outf, "COMMIT;\n");

	fclose(outf);
	return true;
}

