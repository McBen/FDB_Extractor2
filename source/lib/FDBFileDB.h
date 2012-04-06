#pragma once

#include "FDBFile.h"
#include "FDBFieldManager.h"

class FDBFileDB : public  FDBFile
{
	private:
		FDBPackage::file_info f_info;
        FDBFieldManager::s_file_header* head;
        BYTE* entries;

    public:
        FDBFileDB(const FDBPackage::file_info& s_info, BYTE* data );

        FDBPackage::e_export_format DefaultFormat()    { return FDBPackage::EX_CSV; };
        bool ExportFormatIsValid(FDBPackage::e_export_format e)    { return (e==FDBPackage::EX_CSV)||(e==FDBPackage::EX_SQLITE3)||(FDBFile::ExportFormatIsValid(e)); };

		bool WriteCSV(const char*);
		bool WriteSQLITE3(const char*, const char* table_name);
};

