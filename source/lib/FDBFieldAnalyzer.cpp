#include "stdafx.h"

#include "FDBFieldAnalyzer.h"
#include "utils.h"
#include <ctype.h>
using namespace std;


FDBFieldAnalyzer::FDBFieldAnalyzer(const FDBPackage::file_info& s_info, BYTE* _data)
{
	data = _data;
	head = (FDBFieldManager::s_file_header*)data;
	entries = data + sizeof(FDBFieldManager::s_file_header);
	data_end = data + s_info.size_uncomp;

	assert(head->unknown==0x00006396);
}

void FDBFieldAnalyzer::Do( field_list& result )
{
	result.clear();

 	LoadFieldDef(result);
	FillFieldDef(result);
 	TestFieldTypes(result);
}


void FDBFieldAnalyzer::LoadFieldDef( field_list& fields )
{
	fields.clear();

	//-----------------
#pragma warning( disable : 4200 )
#pragma pack(1)
	struct s_field_desc
	{
		int name_len;
		WORD position;
		WORD flag;
		const char name[];
	};
#pragma pack()
#pragma warning( default: 4200 )

	BYTE* run = entries  + head->entry_count * head->entry_size;

	FDB_DBField field;

	while (run < data_end)
	{
		s_field_desc* f = (s_field_desc*)run;
		run += 8 + f->name_len;
		if (f->name_len==0) break;

		if ( !(f->flag&1) && f->position < head->entry_size )
		{
			field.name = f->name;
			field.position = f->position;
			field.type = FDB_DBField::NOF_FIELD_TYPE;
			field.size = 0;

			fields.push_back(field);
		}
	}

	if (fields.size()==0) return;

	sort(fields.begin(), fields.end());

	for (size_t t=0;t<fields.size()-1;++t)
	{
		fields[t].size = fields[t+1].position - fields[t].position;
	}

	fields[ fields.size()-1 ].size = head->entry_size - fields[  fields.size()-1 ].position;
}

void FDBFieldAnalyzer::FillFieldDef( field_list& result )
{
	if (result.size()==0)
	{
		FDB_DBField newf("",FDB_DBField::NOF_FIELD_TYPE,0,head->entry_size);
		result.push_back(newf);
		return;
	}

	if (result[0].position>0)
	{
		FDB_DBField newf("",FDB_DBField::NOF_FIELD_TYPE,0,result[0].position);
		result.insert(result.begin(),newf);
	}

	size_t last_pos = result.rbegin()->position + result.rbegin()->size;
	if (last_pos < head->entry_size)
	{
		FDB_DBField newf("",FDB_DBField::NOF_FIELD_TYPE,last_pos,head->entry_size-last_pos);
		result.insert(result.end(),newf);
	}

}


void FDBFieldAnalyzer::TestFieldTypes(field_list& fields)
{
	ani_list finfo = FieldAnalysis();

REDO:
	for (field_list::iterator field=fields.begin();field!=fields.end();++field)
	{
		if (field->type != FDB_DBField::NOF_FIELD_TYPE) continue;

		int vary_bytes = field->size;
		while (vary_bytes>0 && finfo[field->position+vary_bytes-1].IsConstValue(0) ) --vary_bytes;

		if (vary_bytes==0 && field->name.empty())
		{
			fields.erase(field);
			goto REDO;
		}


		if (field->size==1)
		{
			field->type = FDB_DBField::F_BYTE;
			continue;
		}

		if (field->size<4)
		{
			field->type = FDB_DBField::F_BYTE;

			if (vary_bytes>1)
			{
				for (int i=1;i<vary_bytes;++vary_bytes)
				{
					FDB_DBField newf("", FDB_DBField::F_BYTE, field->position+vary_bytes,1);
					field=fields.insert(field+1,newf);
				}

				goto REDO;
			}

			continue;
		}

		if (field->size>4)
		{
            size_t s = field->size;
            field->type = Best4ByteMatch(finfo, field->position,110);
            field->size=4;

			// string test
			if ( field->type == FDB_DBField::F_STRING) //StringChance(finfo, field->position, vary_bytes) >95 && vary_bytes>3)
			{
                size_t t=1;
                for (;t<s;++t)
                    if (finfo[ field->position + t].IsConstValue(0)) break;
                for (;t<s;++t)
                    if (!finfo[ field->position + t].IsConstValue(0)) break;

				field->size=t;
			}

			FDB_DBField newf("",FDB_DBField::NOF_FIELD_TYPE,field->position+field->size, s-field->size);
			fields.insert(field+1,newf);
			goto REDO;
		}

		if (field->size==4)
		{
			field->type = Best4ByteMatch(finfo, field->position,80);

			if (field->type==FDB_DBField::NOF_FIELD_TYPE)
			{
				if (field->name.empty())	
				{
					fields.erase(field);
					goto REDO;
				}
					
				field->type=FDB_DBField::F_DWORD;
			}

			// 
			if (IsDwordConst(finfo,field->position))
			{
				char temp[64]="";
				DWORD val = GetConstDWORD(finfo, field->position);
				if (field->type==FDB_DBField::F_DWORD) sprintf(temp,"=%u",val);
				if (field->type==FDB_DBField::F_INT)   sprintf(temp,"=%i",val);

				field->SetCommentConst(temp);
			}
		}
	}
}

FDB_DBField::field_type FDBFieldAnalyzer::Best4ByteMatch(ani_list& finfo, DWORD pos, int string_rate)
{

    if (finfo[pos].IsValidType(FDB_DBField::F_BOOL))
    {
        if (IsDwordConstNull(finfo, pos ))	return 	FDB_DBField::NOF_FIELD_TYPE;
        return FDB_DBField::F_BOOL;
    }


    int s_c[FDB_DBField::NOF_FIELD_TYPE];
    s_c[FDB_DBField::F_INT] = finfo[pos].GetCount(FDB_DBField::F_INT);
    s_c[FDB_DBField::F_DWORD] = finfo[pos].GetCount(FDB_DBField::F_DWORD);
    s_c[FDB_DBField::F_FLOAT] = finfo[pos].GetCount(FDB_DBField::F_FLOAT);
    s_c[FDB_DBField::F_STRING] = StringChance(finfo, pos, 4)*string_rate/100;

    FDB_DBField::field_type res = FDB_DBField::F_DWORD;
    if ( s_c[res] < s_c[FDB_DBField::F_INT] )  res = FDB_DBField::F_INT;
    if ( s_c[res] < s_c[FDB_DBField::F_FLOAT] )  res = FDB_DBField::F_FLOAT;
    //if ( s_c[res] < s_c[FDB_DBField::F_DWORD] )  res = FDB_DBField::F_DWORD;
    if ( s_c[res] < s_c[FDB_DBField::F_STRING] ) res = FDB_DBField::F_STRING;

    return res;
}

bool FDBFieldAnalyzer::IsDwordConstNull(ani_list& finfo, DWORD pos )
{
    return  IsDwordConst(finfo, pos) && GetConstDWORD(finfo, pos)==0;
}

bool FDBFieldAnalyzer::IsDwordConst(ani_list& finfo, DWORD pos )
{
    return  finfo[pos].IsConst() &&
            finfo[pos+1].IsConst() &&
            finfo[pos+2].IsConst() &&
            finfo[pos+3].IsConst();
}

DWORD FDBFieldAnalyzer::GetConstDWORD(ani_list& finfo, DWORD pos )
{
	DWORD val = 
			((DWORD)(finfo[pos].GetConstValue())) |
            ((DWORD)(finfo[pos+1].GetConstValue())<<8) |
            ((DWORD)(finfo[pos+2].GetConstValue())<<16) |
            (DWORD)(finfo[pos+3].GetConstValue())<<24;

	return val;
}

int FDBFieldAnalyzer::StringChance(ani_list& finfo, DWORD pos, size_t s)
{
    if (s==0) return 1;

    int res = 0;
    for (size_t t=0;t<s;++t)
    {
        int v =finfo[pos+t].GetCount(FDB_DBField::F_STRING); 
        if (v<0) return -1;
        res += v;
    }

    return res/s;
}

bool IsValidUTF8(BYTE* run,int max_chars)
{
    // U+0000..U+007F     00..7F
    if (*run < 0x20 && *run!=10 && *run!=0 && *run!=13) return false;
    if (*run < 0x80) return true;
    if (*run > 0xf4) return false;

    // U+0080..U+07FF     C2..DF     80..BF
    if (max_chars<2) return false;
    if (*run>=0xc2 && *run<=0xdf)   return (run[1]>=0x80 && run[1]<=0xbf);

    //  U+0800..U+0FFF     E0         A0..BF      80..BF
    if (max_chars<3) return false;
    if (*run==0xe0)   return (run[1]>=0xa0 && run[1]<=0xbf) && (run[2]>=0x80 && run[2]<=0xbf);
    //  U+1000..U+CFFF     E1..EC     80..BF      80..BF
    if (*run>=0xe1 && *run<=0xec)   return (run[1]>=0x80 && run[1]<=0xbf) && (run[2]>=0x80 && run[2]<=0xbf);
    //  U+D000..U+D7FF     ED         80..9F      80..BF
    if (*run==0xed)   return (run[1]>=0x80 && run[1]<=0x9f) && (run[2]>=0x80 && run[2]<=0xbf);
    //   U+E000..U+FFFF     EE..EF     80..BF      80..BF
    if (*run>=0xee && *run<=0xef)   return (run[1]>=0x80 && run[1]<=0xbf) && (run[2]>=0x80 && run[2]<=0xbf);

    //  U+10000..U+3FFFF   F0         90..BF      80..BF     80..BF
    if (max_chars<4) return false;
    if (*run==0xf0)   return (run[1]>=0x90 && run[1]<=0xbf) && (run[2]>=0x80 && run[2]<=0xbf)  && (run[3]>=0x80 && run[3]<=0xbf);
    // U+40000..U+FFFFF   F1..F3     80..BF      80..BF     80..BF
    if (*run>=0xf1 && *run<=0xf3)   return (run[1]>=0x80 && run[1]<=0xbf) && (run[2]>=0x80 && run[2]<=0xbf)  && (run[3]>=0x80 && run[3]<=0xbf);
    // U+100000..U+10FFFF F4         80..8F      80..BF     80..BF
    if (*run==0xf4)   return (run[1]>=0x80 && run[1]<=0x8f) && (run[2]>=0x80 && run[2]<=0xbf) && (run[3]>=0x80 && run[3]<=0xbf);

    return false;
}

std::vector<FDBFieldAnalyzer::aninfo>  FDBFieldAnalyzer::FieldAnalysis()
{
	// prepare
	std::vector<aninfo> binfo;
	binfo.resize(head->entry_size);

	for (DWORD t=0;t<head->entry_size;++t)
	{
		binfo[t].SetConst(entries[t]);
	}

	for (int t=1;t<4;++t)
	{
		binfo[head->entry_size-t].InvalidType(FDB_DBField::F_BOOL);
		binfo[head->entry_size-t].InvalidType(FDB_DBField::F_FLOAT);
		binfo[head->entry_size-t].InvalidType(FDB_DBField::F_INT);
		binfo[head->entry_size-t].InvalidType(FDB_DBField::F_DWORD);
	}


	BYTE* end = entries  + head->entry_count * head->entry_size;
	BYTE* run = entries;
	while (run<end)
	{
        std::vector<aninfo>::iterator cur_byte = binfo.begin();
        for (int rest = head->entry_size; rest>0;--rest,++run,++cur_byte)
		{
			cur_byte->TestConstValue(*run);

			
			if (cur_byte->IsValidType(FDB_DBField::F_BOOL))
			{
				if ((*run==0 || *run==1) && run[1]==0 && run[2]==0 && run[3]==0)
					cur_byte->IncType(FDB_DBField::F_BOOL);
				else  
					cur_byte->InvalidType(FDB_DBField::F_BOOL);
			}

			//cur_byte->count[FDBFieldManager::F_BYTE]++; // do it at the end

            if (cur_byte->IsValidType(FDB_DBField::F_STRING))
            {
                if (!IsValidUTF8(run,rest))  cur_byte->InvalidType(FDB_DBField::F_STRING);
                else
                if (isprint(*run) || !*run) cur_byte->IncType(FDB_DBField::F_STRING);
            }

            if (rest>3)
            {
			    if (cur_byte->IsValidType(FDB_DBField::F_FLOAT))
			    {
				    if ((*(FLOAT*)run) != (*(FLOAT*)run))
				    {
					    cur_byte->InvalidType(FDB_DBField::F_FLOAT);
				    }
				    else
					    if (fabs(*(FLOAT*)run)<1e10) cur_byte->IncType(FDB_DBField::F_FLOAT);

			    }

			    if (cur_byte->IsValidType(FDB_DBField::F_INT) &&   (*(int*)run)  <0x08000000) cur_byte->IncType(FDB_DBField::F_INT);
			    if (cur_byte->IsValidType(FDB_DBField::F_DWORD) && (*(DWORD*)run)<0x0fffffff) cur_byte->IncType(FDB_DBField::F_DWORD);
            }
		}
	}


	// 
	for (DWORD t=0;t<head->entry_size;++t)
	{
        binfo[t].ConvertToPercent(head->entry_count);
	}

	return binfo;
}


FDBFieldAnalyzer::aninfo::aninfo()
{
	is_const=true;
	const_value=0;
	ZeroMemory(&count, FDB_DBField::NOF_FIELD_TYPE*sizeof(DWORD) );
}

void FDBFieldAnalyzer::aninfo::SetConst(BYTE v)
{
	is_const=true;
	const_value=v;
}

BYTE FDBFieldAnalyzer::aninfo::GetConstValue()
{
	assert(is_const);
	return const_value;
}

bool FDBFieldAnalyzer::aninfo::IsConstValue(BYTE v)
{
	return is_const && const_value==v;
}

bool FDBFieldAnalyzer::aninfo::IsConst()
{
	return is_const;
}

void FDBFieldAnalyzer::aninfo::TestConstValue(BYTE v)
{
	if  (const_value!=v) is_const=false;
}

void FDBFieldAnalyzer::aninfo::InvalidType(FDB_DBField::field_type t)
{
	count[t]=-1;
}

bool FDBFieldAnalyzer::aninfo::IsValidType(FDB_DBField::field_type t)
{
	return count[t]>=0;
}

void FDBFieldAnalyzer::aninfo::IncType(FDB_DBField::field_type t)
{
	if (IsValidType(t))		++count[t];
}

int  FDBFieldAnalyzer::aninfo::GetCount(FDB_DBField::field_type t)
{
	return count[t];
}

void FDBFieldAnalyzer::aninfo::ConvertToPercent(int max_val)
{
	for (int i=0;i<FDB_DBField::NOF_FIELD_TYPE;++i)
	{
		if (count[i]>=0)
		{
			count[i] = (int)  ( (float) count[i] / max_val *100.0f );
		}
	}

    count[FDB_DBField::F_BYTE] = 100;
}
