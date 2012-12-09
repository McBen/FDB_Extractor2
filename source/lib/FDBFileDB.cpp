#include "stdafx.h"

#include "FDBFileDB.h"
#include "FDBFieldManager.h"
#include "ExportFormat.h"
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
	DBExport_CSV out(filename);
	return DoExport(out, NULL);
}

bool FDBFileDB::WriteSQLITE3(const char* filename)
{
	boost::filesystem::path filepath(filename);
	const char* table_name = filepath.stem().generic_string().c_str();

	DBExport_Sqlite3 out(filename);
	return DoExport(out, table_name);
}

bool FDBFileDB::DoExport(DBExport& exporter, const char* table_name)
{
	field_list* fields = g_field_manager.GetFieldDefinition(f_info, data);
	return ExportTable(exporter, table_name, fields);
}

bool FDBFileDB::ExportTable(DBExport& exporter, const char* table_name, field_list* fields)
{
	exporter.TableStart(table_name);
	for (field_list::iterator field=fields->begin();field!=fields->end();++field)
	{
		exporter.TableField(field->name,field->position,field->type,field->size);
	}
	exporter.TableEnd();


	for (DWORD idx=0;idx<head->entry_count;++idx)
	{
        exporter.EntryStart();

		BYTE* line = entries+idx*head->entry_size;

		for (field_list::iterator field=fields->begin();field!=fields->end();++field)
		{
			exporter.EntryField(field->type,line+field->position);
		}
		exporter.EntryEnd();
	}

	return true;
}

FDB_DBField* FindField(field_list* fields, const string&name)
{
	for (field_list::iterator field=fields->begin();field!=fields->end();++field)
	{
		if (field->name==name) return &field[0];
	}
	return NULL;
}

bool FDBFileDB_LearnMagic::DoExport(DBExport& exporter, const char* table_name)
{
	field_list* fields = g_field_manager.GetFieldDefinition(f_info, data);
	bool r = ExportTable(exporter, table_name, fields);
	if (!r) return false;


	FDB_DBField* spmagicinfo = FindField(fields,"spmagicinfo");
	FDB_DBField* spmagiccount = FindField(fields,"spmagiccount");
	if (spmagicinfo)
	{
		exporter.TableStart("spmagicinfo");
		exporter.TableField((*fields)[0].name, (*fields)[0].position, (*fields)[0].type, (*fields)[0].size);
		exporter.TableField("id",4,FDB_DBField::F_DWORD,4);
		exporter.TableField("i1",4,FDB_DBField::F_DWORD,4);
		exporter.TableField("i2",4,FDB_DBField::F_DWORD,4);
		exporter.TableField("i3",4,FDB_DBField::F_DWORD,4);
		exporter.TableField("i4",4,FDB_DBField::F_DWORD,4);
		exporter.TableField("i5",4,FDB_DBField::F_DWORD,4);
		exporter.TableField("i6",4,FDB_DBField::F_DWORD,4);
		exporter.TableEnd();

		for (DWORD idx=0;idx<head->entry_count;++idx)
		{
			BYTE* line = entries+idx*head->entry_size;
			DWORD count = *(DWORD*)(line+spmagiccount->position);
			assert(count < spmagicinfo->size/(6*4));

			for (DWORD i=0;i<count;++i)
			{
				exporter.EntryStart();
				exporter.EntryField((*fields)[0].type, line+ (*fields)[0].position);
				DWORD temp = i+1;
				exporter.EntryField(FDB_DBField::F_DWORD,&temp);

				for (DWORD t=0;t<6;++t)
					exporter.EntryField(FDB_DBField::F_DWORD,line + spmagicinfo->position+t*4 + i*6*4);
				exporter.EntryEnd();
			}
		}
	}

	FDB_DBField* normalmagicinfo = FindField(fields,"normalmagicinfo");
	FDB_DBField* normalmagiccount = FindField(fields,"normalmagiccount");
	if (normalmagicinfo)
	{
		exporter.TableStart("normalmagicinfo");
		exporter.TableField((*fields)[0].name, (*fields)[0].position, (*fields)[0].type, (*fields)[0].size);
		exporter.TableField("id",4,FDB_DBField::F_DWORD,4);
		exporter.TableField("i1",4,FDB_DBField::F_DWORD,4);
		exporter.TableField("i2",4,FDB_DBField::F_DWORD,4);
		exporter.TableField("i3",4,FDB_DBField::F_DWORD,4);
		exporter.TableField("i4",4,FDB_DBField::F_DWORD,4);
		exporter.TableField("i5",4,FDB_DBField::F_DWORD,4);
		exporter.TableField("i6",4,FDB_DBField::F_DWORD,4);
		exporter.TableEnd();

		for (DWORD idx=0;idx<head->entry_count;++idx)
		{
			BYTE* line = entries+idx*head->entry_size;
			DWORD count = *(DWORD*)(line+normalmagiccount->position);
			assert(count < normalmagicinfo->size/(6*4));

			for (DWORD i=0;i<count;++i)
			{
				exporter.EntryStart();
				exporter.EntryField((*fields)[0].type, line+ (*fields)[0].position);
				DWORD temp = i+1;
				exporter.EntryField(FDB_DBField::F_DWORD,&temp);

				for (DWORD t=0;t<6;++t)
					exporter.EntryField(FDB_DBField::F_DWORD,line + normalmagicinfo->position+t*4 + i*6*4);
				exporter.EntryEnd();
			}
		}
	}

	return true;
}




