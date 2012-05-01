#include "stdafx.h"

#include "FDBFieldManager.h"
#include "FDBFieldAnalyzer.h"
#include "utils.h"
#include "csv_parser.hpp"

#include <boost/algorithm/string.hpp>

#include <iostream>
#include <iomanip>
#include <ctype.h>
#include <algorithm>
using namespace std;


FDBFieldManager g_field_manager;


FDBFieldManager::FDBFieldManager()
{
	state = E_NOT_INIT;
}

FDBFieldManager::~FDBFieldManager()
{
	if (state == E_CHANGED) WriteCSV();
}

field_list* FDBFieldManager::GetFieldDefinition(const FDBPackage::file_info& s_info, BYTE* data )
{
	if (state == E_NOT_INIT) Load();

	FDBFieldDef new_entry;

	field_def_list::iterator ent = FindEntry(s_info.name);
	if (ent != cache.end())
	{
		if (ent->head_crc == s_info.size_uncomp)
			return &(ent->fields);

		new_entry = AnalyseFile(s_info, data);
		Merge(new_entry, *ent);
		cache.erase(ent);
	}
	else
		new_entry = AnalyseFile(s_info, data);


	cache.push_back(new_entry);
	state= E_CHANGED;

	ent = FindEntry(s_info.name);
	assert(ent != cache.end());
	if (ent == cache.end())	return &empty_field_list;
	
	return &(ent->fields);
}


FDBFieldDef FDBFieldManager::AnalyseFile(const FDBPackage::file_info& s_info, BYTE* data)
{
	FDBFieldDef new_entry;
	new_entry.filename =s_info.name;
	new_entry.head_crc =s_info.size_uncomp;

	FDBFieldAnalyzer analyse(s_info,data);
	analyse.Do(new_entry.fields);

	return new_entry;
}

void FDBFieldManager::Merge(FDBFieldDef& cur, const FDBFieldDef& old_def)
{
	for (field_list::const_iterator old_field=old_def.fields.begin(); old_field!=old_def.fields.end();++old_field)
	{
		FDB_DBField* new_field = cur.GetFieldAt(old_field->position);
		if (new_field)
		{
			if (new_field->name.empty())    new_field->name = old_field->name;

			if (!new_field->HasCustomComment() && old_field->HasCustomComment()) 
			{
				if (new_field->name == old_field->name)
					new_field->SetComment(old_field->comment);
				else
					new_field->SetComment(string("?? ") + old_field->comment);
			}
		}
		else
		{
			assert(! cur.IsUndefined(old_field->position, old_field->size));
			// TODO: create new field
		}

	}
}


field_def_list::iterator FDBFieldManager::FindEntry(const string& name)
{
	field_def_list::iterator e=cache.begin();
	for (;e!=cache.end();++e)
		if (name == e->filename) break;

	return e;
}

void FDBFieldManager::Load()
{
	assert(state == E_NOT_INIT);

	cache.clear();

	LoadCSV();
	state = E_LOADED;
}

DWORD Hex2Dec(const char* r)
{
	DWORD res =0;
	while (*r)
	{
		int v=*r-'0';
		if (v>='a'-'0' && v<='f'-'0') v+='0'-'a'+10;
		else if (v>='A'-'0'&& v<='F'-'0') v+='0'-'A'+10;
		res = (res<<4) + v;
		++r;
	};
	return res;
}

void FDBFieldManager::LoadCSV()
{
	vector<string> row;

	ifstream csv("db_crc.csv");
	while (csvparse(csv,row)) {
		FDBFieldDef new_db;
		new_db.filename = row[0];
		new_db.head_crc = Hex2Dec(row[1].c_str());

		cache.push_back(new_db);
	}
	csv.close();

	csv.open("fields.csv");
	while (csvparse(csv,row)) {

		field_def_list::iterator fields = FindEntry(row[0]);
		if (fields==cache.end())
		{
			FDBFieldDef new_db;
			new_db.filename = row[0];
			new_db.head_crc = 0;
			cache.push_back(new_db);
			fields = FindEntry(row[0]);
		}

		FDB_DBField new_field;
		new_field.name = row[1];
		boost::algorithm::trim(new_field.name);
		new_field.position = Hex2Dec(row[2].c_str());
		new_field.size = Hex2Dec(row[3].c_str());
		new_field.type = (FDB_DBField::field_type)atoi(row[4].c_str());
		if (row.size()>4)	new_field.comment = row[5];

		fields->fields.push_back(new_field);
	}

	for (field_def_list::iterator i=cache.begin();i!=cache.end();++i)
	{
		sort(i->fields.begin(),i->fields.end());
	}
}

void FDBFieldManager::WriteCSV()
{
	sort(cache.begin(),cache.end());
	for (field_def_list::iterator i=cache.begin();i!=cache.end();++i)
	{
		sort(i->fields.begin(),i->fields.end());
	}

	ofstream outf_crc("db_crc.csv");
	for (field_def_list::iterator db=cache.begin();db!=cache.end();++db)
	{
		outf_crc << db->filename << ";" << setw(8) << setfill('0') << hex << db->head_crc << "\n";
	}


	ofstream outf("fields.csv");

	for (field_def_list::iterator db=cache.begin();db!=cache.end();++db)
	{
		for (field_list::iterator field=db->fields.begin();field!=db->fields.end();++field)
		{
			string comment = EscapeCSV_String(field->comment);

			outf << db->filename << ";" << field->name<<";" 
				<< setw(4)<<setfill('0') <<hex<< field->position<<";"
				<< setw(2)<<setfill('0') <<hex<< field->size<<";"
				<< field->type<<";"<<comment<<"\n";
		}
	}

	state = E_WRITTEN;
}
