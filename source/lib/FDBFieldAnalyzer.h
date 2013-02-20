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
				uint8_t const_value;
				int  count[FDB_DBField::NOF_FIELD_TYPE];

				// TODO:
				uint32_t chars_related; // (the char before:) 0-independent (prev. was 0); 1-connected ; 2-strong_connection (utf-8 char)

			public:
				aninfo();
				void SetConst(uint8_t v);
				void TestConstValue(uint8_t v);
				uint8_t GetConstValue();

				void IncType(FDB_DBField::field_type);
				void InvalidType(FDB_DBField::field_type);

				void ConvertToPercent(int max_val);

				bool IsConst();
				bool IsConstValue(uint8_t v);
				bool IsValidType(FDB_DBField::field_type);
				int  GetCount(FDB_DBField::field_type);
		};
		typedef std::vector<aninfo> ani_list;

		
		FDBFieldManager::s_file_header* head;
		uint8_t* entries;
		uint8_t* data;
		uint8_t* data_end;

		void LoadFieldDef( field_list& result );
		void FillFieldDef( field_list& result );
		void TestFieldTypes( field_list& result );
        int StringChance(ani_list& finfo, uint32_t pos, size_t s);

        FDB_DBField::field_type Best4ByteMatch(ani_list& finfo, uint32_t pos, int string_rate);
        bool IsDwordConstNull(ani_list& finfo, uint32_t pos );
		bool IsDwordConst(ani_list& finfo, uint32_t pos );
		uint32_t GetConstDWORD(ani_list& finfo, uint32_t pos );

		ani_list  FieldAnalysis();

	public:
		FDBFieldAnalyzer(const FDBPackage::file_info& s_info, uint8_t* data);

		void Do(field_list& result );

};

