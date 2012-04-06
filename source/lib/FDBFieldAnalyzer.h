#pragma once

#include "FDBFieldManager.h"


class FDBFieldAnalyzer
{
	private:
		class aninfo
		{
			public:

			private:
				bool is_const;
				BYTE const_value;
				int  count[FDB_DBField::NOF_FIELD_TYPE];

				// TODO:
				DWORD chars_related; // (the char before:) 0-independent (prev. was 0); 1-connected ; 2-strong_connection (utf-8 char)

			public:
				aninfo();
				void SetConst(BYTE v);
				void TestConstValue(BYTE v);
				BYTE GetConstValue();

				void IncType(FDB_DBField::field_type);
				void InvalidType(FDB_DBField::field_type);

				void ConvertToPercent(int max_val);

				bool IsConst();
				bool IsConstValue(BYTE v);
				bool IsValidType(FDB_DBField::field_type);
				int  GetCount(FDB_DBField::field_type);
		};
		typedef std::vector<aninfo> ani_list;

		
		FDBFieldManager::s_file_header* head;
		BYTE* entries;
		BYTE* data;
		BYTE* data_end;

		void LoadFieldDef( field_list& result );
		void FillFieldDef( field_list& result );
		void TestFieldTypes( field_list& result );
        int StringChance(ani_list& finfo, DWORD pos, size_t s);

        FDB_DBField::field_type Best4ByteMatch(ani_list& finfo, DWORD pos, int string_rate);
        bool IsDwordConstNull(ani_list& finfo, DWORD pos );
		bool IsDwordConst(ani_list& finfo, DWORD pos );
		DWORD GetConstDWORD(ani_list& finfo, DWORD pos );

		ani_list  FieldAnalysis();

	public:
		FDBFieldAnalyzer(const FDBPackage::file_info& s_info, BYTE* data);

		void Do(field_list& result );

};

