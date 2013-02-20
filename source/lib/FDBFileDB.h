#pragma once

#include "FDBFile.h"
#include "FDBFieldManager.h"
#include "ExportFormat.h"


class FDBFileDB : public  FDBFile
{
	protected:
		FDBPackage::file_info f_info;
        FDBFieldManager::s_file_header* head;
        uint8_t* entries;

    public:
        FDBFileDB(const FDBPackage::file_info& s_info, uint8_t* data );

        FDBPackage::e_export_format DefaultFormat()    { return FDBPackage::EX_CSV; };
        bool ExportFormatIsValid(FDBPackage::e_export_format e)    { return (e==FDBPackage::EX_CSV)||(e==FDBPackage::EX_SQLITE3)||(FDBFile::ExportFormatIsValid(e)); };

		bool WriteCSV(const char*);
		bool WriteSQLITE3(const char*);

	protected:
		virtual bool DoExport(DBExport& exporter, const char* table_name);
		bool ExportTable(DBExport& exporter, const char* table_name, field_list* fields);
};


class FDBFileDB_LearnMagic : public FDBFileDB
{
	public:
		FDBFileDB_LearnMagic(const FDBPackage::file_info& s_info, uint8_t* data ) : FDBFileDB(s_info, data ) {};

	protected:
		bool DoExport(DBExport& exporter, const char* table_name);
		bool WriteSubArray(DBExport& exporter, field_list* fields, std::string name);
};
