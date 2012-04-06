#include "stdafx.h"

#include "FDBFileDB.h"
#include "FDBFieldManager.h"
#include "utils.h"
#include <iostream>
#include <iomanip>
#include <ctype.h>
using namespace std;

FDBFileDB::FDBFileDB(const FDBPackage::file_info& s_info, BYTE* data )
    : FDBFile(s_info,data)
{
	f_info = s_info;
    head = (FDBFieldManager::s_file_header*)data;
    entries = data + sizeof(FDBFieldManager::s_file_header);

    assert(head->unknown==0x00006396);
}

bool FDBFileDB::WriteCSV(const char* filename)
{
	field_list* fields = g_field_manager.GetFieldDefinition(f_info, data);


	FILE* outf;
	std::string name(filename);
	if (fopen_s(&outf, filename, "wt")) return false;

	// Head
	for (field_list::iterator field=fields->begin();field!=fields->end();++field)
	{
		if (field!=fields->begin()) fprintf(outf,";");

		if (field->name.empty()) 
			fprintf(outf,"unk%i",field->position);
		else
			fprintf(outf,"%s",field->name.c_str());
	}
	fprintf(outf,"\n");

	// Entries
	for (DWORD idx=0;idx<head->entry_count;++idx)
	{
		BYTE* line = entries+idx*head->entry_size;

		for (field_list::iterator field=fields->begin();field!=fields->end();++field)
		{
			if (field!=fields->begin()) fprintf(outf,";");

			switch (field->type)
			{
			case FDB_DBField::F_BYTE:
				fprintf(outf,"%i",line[field->position]);
				break; 
			case FDB_DBField::F_INT:
				fprintf(outf,"%i",*(int*)(line+field->position));
				break;                       
			case FDB_DBField::F_BOOL:
			case FDB_DBField::F_DWORD:
				fprintf(outf,"%u",*(unsigned int*)(line+field->position));
				break;                       
			case FDB_DBField::F_FLOAT:
				fprintf(outf,"%g",*(float*)(line+ field->position));
				break; 

			case FDB_DBField::F_STRING: 
				fputs(EscapeCSV_String((char*)(line+ field->position)).c_str(), outf);
				break;
			default:
				assert(false);
			}
		}
		fprintf(outf,"\n");
	}

	fclose(outf);

	return true;
}

bool FDBFileDB::WriteSQLITE3(const char* filename, const char* table_name)
{
	field_list* fields = g_field_manager.GetFieldDefinition(f_info, data);

	FILE* outf;
	string name(filename);
	if (fopen_s(&outf, filename, "wt")) return false;


	// Options
    boost::filesystem::path filepath(filename);
	fprintf(outf, "-- %s\n", filepath.filename().generic_string().c_str());
	//fprintf(outf, "/*!40101 SET NAMES utf8 */;\n");
	fprintf(outf, "\n");
	
	// Head
    fprintf(outf, "BEGIN TRANSACTION;\n");
    
	fprintf(outf, "DROP TABLE IF EXISTS `%s`;\n", table_name);
	fprintf(outf, "CREATE TABLE `%s` (\n", table_name);

	for (field_list::iterator field=fields->begin();field!=fields->end();++field)
	{
		if (field->name.empty()) 
			fprintf(outf," `unk%i` ",field->position);
		else
			fprintf(outf," `%s` ",field->name.c_str());

		switch (field->type)
		{
			case FDB_DBField::F_BOOL:   fprintf(outf,"BOOLEAN"); break;
			case FDB_DBField::F_BYTE:   fprintf(outf,"TINYINT"); break; 
			case FDB_DBField::F_INT:    fprintf(outf,"INTEGER");break; 
			case FDB_DBField::F_DWORD:  fprintf(outf,"INTEGER UNSIGNED");break; 
			case FDB_DBField::F_FLOAT:  fprintf(outf,"REAL"); break; 
			case FDB_DBField::F_STRING: fprintf(outf,"CHAR(%i)",field->size); break;
			default:
				assert(false);
		}

        if (field+1!=fields->end()) fprintf(outf,",");

		fprintf(outf,"\n");
	}

	fprintf(outf,");\n\n");


    // Entries
	for (DWORD idx=0;idx<head->entry_count;++idx)
	{
        fprintf(outf,"INSERT INTO `%s` VALUES (",table_name);

		BYTE* line = entries+idx*head->entry_size;

		for (field_list::iterator field=fields->begin();field!=fields->end();++field)
		{
			if (field!=fields->begin()) fprintf(outf,",");

			switch (field->type)
			{
			case FDB_DBField::F_BYTE:
				fprintf(outf,"%i",line[field->position]);
				break; 
			case FDB_DBField::F_INT:
				fprintf(outf,"%i",*(int*)(line+field->position));
				break;                       
			case FDB_DBField::F_BOOL:
			case FDB_DBField::F_DWORD:
				fprintf(outf,"%u",*(unsigned int*)(line+field->position));
				break;                       
			case FDB_DBField::F_FLOAT:
				fprintf(outf,"%g",*(float*)(line+ field->position));
				break; 

			case FDB_DBField::F_STRING: 
				fprintf(outf,"'%s'", EscapeSQLITE3_String((char*)(line+ field->position)).c_str());
				break;
			default:
				assert(false);
			}
		}
		fprintf(outf,");\n");
	}

    fprintf(outf, "\nEND TRANSACTION;\n");

	fclose(outf);

	return true;
}




