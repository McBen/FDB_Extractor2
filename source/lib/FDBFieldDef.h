#pragma once

#include "FDBField.h"
#include <vector>

class FDBFieldDef
{
	public:
		std::string filename;
		DWORD head_crc;
		field_list fields;

	FDB_DBField* GetFieldAt(DWORD pos);
	bool	IsUndefined(DWORD pos, size_t size);

	bool operator<(const FDBFieldDef& j) const { return (filename < j.filename);}

};

typedef std::vector<FDBFieldDef> field_def_list;


