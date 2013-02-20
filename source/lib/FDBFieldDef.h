#pragma once

#include "FDBField.h"
#include <vector>

class FDBFieldDef
{
	public:
		std::string filename;
		uint32_t   head_crc;
		field_list fields;

	FDB_DBField* GetFieldAt(uint32_t pos);
	bool	IsUndefined(uint32_t pos, size_t size);

	bool operator<(const FDBFieldDef& j) const { return (filename < j.filename);}

};

typedef std::vector<FDBFieldDef> field_def_list;


