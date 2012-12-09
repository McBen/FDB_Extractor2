
#include "stdafx.h"
#include "ExportFormat.h"
#include "utils.h"
using namespace std;

DBExport::DBExport(const std::string& _filename)
{
	filename=_filename;
	outf=NULL;
}

DBExport::~DBExport()
{
	Close();
}

void DBExport::Close()
{
	if (outf) fclose(outf);
	outf=NULL;
}

void DBExport::TableStart(const char* table_name) 
{
	if (fopen_s(&outf, filename.c_str(), "wt"))	outf=NULL;
	not_first_value = false;
}

DBExport_CSV::DBExport_CSV(const std::string& _filename) : DBExport(_filename)
{
	realfilename = _filename;
}

void DBExport_CSV::TableStart(const char* _table_name) 
{
	Close();
	if (_table_name && _table_name[0])
	{
		boost::filesystem::path r(realfilename);
		filename = (r.parent_path()/r.stem()).generic_string()+string("_")+string(_table_name)+r.extension().generic_string();
	}

	DBExport::TableStart(_table_name);
}

void DBExport_CSV::TableField(const std::string& name, DWORD position, FDB_DBField::field_type type, size_t size ) 
{
	if (not_first_value) fprintf(outf,";");
	else not_first_value = true;

	if (name.empty()) 
		fprintf(outf,"unk%i",position);
	else
		fprintf(outf, name.c_str());
}

void DBExport_CSV::TableEnd()
{
	fprintf(outf,"\n");
}

void DBExport_CSV::EntryStart() 
{
	not_first_value = false;
}

void DBExport_CSV::EntryField(FDB_DBField::field_type type, void*data) 
{
	if (not_first_value) fprintf(outf,";");
	else not_first_value = true;

	switch (type)
	{
		case FDB_DBField::F_BYTE:
			fprintf(outf,"%i",*(BYTE*)data);
			break; 
		case FDB_DBField::F_INT:
			fprintf(outf,"%i",*(int*)data);
			break;                       
		case FDB_DBField::F_BOOL:
		case FDB_DBField::F_DWORD:
			fprintf(outf,"%u",*(unsigned int*)data);
			break;                       
		case FDB_DBField::F_FLOAT:
			fprintf(outf,"%g",*(float*)data);
			break; 

		case FDB_DBField::F_STRING: 
			fputs(EscapeCSV_String((char*)data).c_str(), outf);
			break;

		case FDB_DBField::F_ARRAY:
			fprintf(outf,"?array?");
			break; 

		default:
			assert(false);
	}
}

void DBExport_CSV::EntryEnd() 
{
	fprintf(outf,"\n");
}


DBExport_Sqlite3::DBExport_Sqlite3(const std::string& _filename) : DBExport(filename)
{
	// Options
	boost::filesystem::path filepath(filename);
	fprintf(outf, "-- %s\n", filepath.filename().generic_string().c_str());
	//fprintf(outf, "/*!40101 SET NAMES utf8 */;\n");
	fprintf(outf, "\n");
}

void DBExport_Sqlite3::TableStart(const char* _table_name)  
{
	DBExport::TableStart(_table_name);

	// TODO: fprintf(outf, "BEGIN TRANSACTION;\n");
	//    fprintf(outf, "\nEND TRANSACTION;\n");

	table_name = _table_name;
	fprintf(outf, "DROP TABLE IF EXISTS `%s`;\n", table_name);
	fprintf(outf, "CREATE TABLE `%s` (\n", table_name);
}

void DBExport_Sqlite3::TableField(const std::string& name, DWORD position, FDB_DBField::field_type type, size_t size) 
{
	if (not_first_value) fprintf(outf,",\n");
	else not_first_value = true;

	if (name.empty()) 
		fprintf(outf," `unk%i` ",position);
	else
		fprintf(outf," `%s` ",name.c_str());

	switch (type)
	{
		case FDB_DBField::F_BOOL:   fprintf(outf,"BOOLEAN"); break;
		case FDB_DBField::F_BYTE:   fprintf(outf,"TINYINT"); break; 
		case FDB_DBField::F_INT:    fprintf(outf,"INTEGER");break; 
		case FDB_DBField::F_DWORD:  fprintf(outf,"INTEGER UNSIGNED");break; 
		case FDB_DBField::F_FLOAT:  fprintf(outf,"REAL"); break; 
		case FDB_DBField::F_STRING: fprintf(outf,"CHAR(%i)",size); break;
		case FDB_DBField::F_ARRAY:  fprintf(outf,"CHAR(%i)",size); break; // TODO: make BLOB
		default:
			assert(false);
	}
}

void DBExport_Sqlite3::TableEnd()
{
	fprintf(outf,"\n);\n\n");
}

void DBExport_Sqlite3::EntryStart() 
{
	not_first_value = false;
	fprintf(outf,"INSERT INTO `%s` VALUES (",table_name);
}

void DBExport_Sqlite3::EntryField(FDB_DBField::field_type type, void*data) 
{
	if (not_first_value) fprintf(outf,",");
	else not_first_value = true;

	switch (type)
	{
		case FDB_DBField::F_BYTE:
			fprintf(outf,"%i",*(BYTE*)data);
			break; 
		case FDB_DBField::F_INT:
			fprintf(outf,"%i",*(int*)data);
			break;                       
		case FDB_DBField::F_BOOL:
		case FDB_DBField::F_DWORD:
			fprintf(outf,"%u",*(unsigned int*)data);
			break;                       
		case FDB_DBField::F_FLOAT:
			fprintf(outf,"%g",*(float*)data);
			break; 
		case FDB_DBField::F_STRING: 
			fprintf(outf,"'%s'", EscapeSQLITE3_String((char*)data).c_str());
			break;
		case FDB_DBField::F_ARRAY:
			fprintf(outf,"'???'");
			break; 

		default:
			assert(false);
	}
}

void DBExport_Sqlite3::EntryEnd() 
{
	fprintf(outf,");\n");
}

