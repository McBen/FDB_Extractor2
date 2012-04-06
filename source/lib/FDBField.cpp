#include "stdafx.h"

#include "FDBField.h"

#include <iostream>
#include <iomanip>
#include <ctype.h>
#include <algorithm>
using namespace std;


field_list empty_field_list; 


FDB_DBField::FDB_DBField()
{
}

FDB_DBField::FDB_DBField(const string& _name, field_type _type, DWORD _pos, DWORD _size)
{
	name = _name;
	type = _type;
	position = _pos;
	size = _size;
}

bool FDB_DBField::HasComment() const
{
	return (!comment.empty());
}

bool FDB_DBField::HasCustomComment() const
{
	return HasComment() && !IsConstCommment();
}

void FDB_DBField::SetCommentConst(const string& _comment)
{
	if (comment.empty() && !_comment.empty())
	{
		if (_comment[0]!='=') 
			comment = string("=")+_comment;
		else 
			comment = _comment;
	}
}

bool FDB_DBField::IsConstCommment() const
{
	if (comment[0]!='=') return false;
	for (size_t i=1;i<comment.size();++i)
		if (!isdigit(comment[i]) && !iswspace(comment[i])) return false;

	return true;
}


void FDB_DBField::SetComment(const string& _comment)
{
	comment = _comment;
}

bool FDB_DBField::operator<(const FDB_DBField& j)
{
	return (position < j.position);
}