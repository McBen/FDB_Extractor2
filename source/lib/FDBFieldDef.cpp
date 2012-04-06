#include "stdafx.h"
#include "FDBFieldDef.h"

#include <iostream>
#include <iomanip>
using namespace std;


FDB_DBField* FDBFieldDef::GetFieldAt(DWORD pos)
{
	for (field_list::iterator i= fields.begin(); i!=fields.end();++i)
	{
		if (i->position==pos) return &i[0];
	}

	return NULL;
}


bool	FDBFieldDef::IsUndefined(DWORD pos, size_t size)
{
	for (field_list::const_iterator i= fields.begin(); i!=fields.end();++i)
	{
		if (i->position+i->size>=pos || i->position<=pos+size) return false;
	}

	return true;
}
