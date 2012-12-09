#include "stdafx.h"

#include "FDBFileDB.h"
#include "FDBFieldManager.h"
#include "utils.h"
#include <iostream>
#include <iomanip>
#include <ctype.h>
using namespace std;


class DBExport
{
	protected:
		FILE* outf;
		const char* table_name;
		bool not_first_value;

	public:
		DBExport(const char* filename, const char* _table_name);
		~DBExport();

		virtual void TableStart() {}
		virtual void TableField(const std::string& name, DWORD position, FDB_DBField::field_type type, size_t size ) {}
		virtual void TableEnd() {}
		virtual void EntryStart() {}
		virtual void EntryField(FDB_DBField::field_type type, void*data) {}
		virtual void EntryEnd() {}
};

DBExport::DBExport(const char* filename, const char* _table_name )
{
	if (fopen_s(&outf, filename, "wt"))	outf=NULL;
	table_name =  _table_name;
}

DBExport::~DBExport()
{
	if (outf) fclose(outf);
}

class DBExport_CSV : public DBExport
{
	public:
		DBExport_CSV(const char* filename, const char* _table_name) : DBExport(filename,_table_name)
		{
		}

		void TableStart()
		{
			not_first_value = false;
		}

		void TableField(const std::string& name, DWORD position, FDB_DBField::field_type type, size_t size ) 
		{
			if (not_first_value) fprintf(outf,";");
			else not_first_value = true;

			if (name.empty()) 
				fprintf(outf,"unk%i",position);
			else
				fprintf(outf, name.c_str());
		}

		void TableEnd()
		{
			fprintf(outf,"\n");
		}

		void EntryStart() 
		{
			not_first_value = false;
		}

		void EntryField(FDB_DBField::field_type type, void*data) 
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
				default:
					assert(false);
			}
		}

		void EntryEnd() 
	{
		fprintf(outf,"\n");
	}
};

class DBExport_Sqlite3 : public DBExport
{
	public:
		DBExport_Sqlite3(const char* filename, const char* _table_name) : DBExport(filename,_table_name)
		{
			// Options
			boost::filesystem::path filepath(filename);
			fprintf(outf, "-- %s\n", filepath.filename().generic_string().c_str());
			//fprintf(outf, "/*!40101 SET NAMES utf8 */;\n");
			fprintf(outf, "\n");
		}

		void TableStart()
		{
			not_first_value = false;

			// TODO: fprintf(outf, "BEGIN TRANSACTION;\n");
			//    fprintf(outf, "\nEND TRANSACTION;\n");

			fprintf(outf, "DROP TABLE IF EXISTS `%s`;\n", table_name);
			fprintf(outf, "CREATE TABLE `%s` (\n", table_name);
		}

		void TableField(const std::string& name, DWORD position, FDB_DBField::field_type type, size_t size) 
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
				default:
					assert(false);
			}
		}

		void TableEnd()
		{
			fprintf(outf,"\n);\n\n");
		}

		void EntryStart() 
		{
			not_first_value = false;
			fprintf(outf,"INSERT INTO `%s` VALUES (",table_name);
		}

		void EntryField(FDB_DBField::field_type type, void*data) 
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
				default:
					assert(false);
			}
		}

		void EntryEnd() 
		{
			fprintf(outf,");\n");
		}
};


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

	DBExport_CSV out(filename,"");

	// Head
	out.TableStart();
	for (field_list::iterator field=fields->begin();field!=fields->end();++field)
	{
		out.TableField(field->name,field->position,field->type,field->size);
	}
	out.TableEnd();

	// Entries
	for (DWORD idx=0;idx<head->entry_count;++idx)
	{
		out.EntryStart();

		BYTE* line = entries+idx*head->entry_size;

		for (field_list::iterator field=fields->begin();field!=fields->end();++field)
		{
			out.EntryField(field->type,line+field->position);
		}
		out.EntryEnd();
	}


	return true;
}

bool FDBFileDB::WriteSQLITE3(const char* filename, const char* table_name)
{
	field_list* fields = g_field_manager.GetFieldDefinition(f_info, data);

	DBExport_Sqlite3 out(filename, table_name);


	out.TableStart();
	for (field_list::iterator field=fields->begin();field!=fields->end();++field)
	{
		out.TableField(field->name,field->position,field->type,field->size);
	}
	out.TableEnd();


    // Entries
	for (DWORD idx=0;idx<head->entry_count;++idx)
	{
        out.EntryStart();

		BYTE* line = entries+idx*head->entry_size;

		for (field_list::iterator field=fields->begin();field!=fields->end();++field)
		{
			out.EntryField(field->type,line+field->position);
		}
		out.EntryEnd();
	}


	return true;
}




