#pragma once

#include "FDBPackage.h"

class FDBFile
{
    public:
        FDBFile(const FDBPackage::file_info& s_info, uint8_t* data );
        virtual ~FDBFile();

		std::string GetTargetName(const char* filename, FDBPackage::e_export_format e=FDBPackage::EX_NONE);
        bool WriteToFile(const char*, FDBPackage::e_export_format e=FDBPackage::EX_NONE);
        virtual bool ExportFormatIsValid(FDBPackage::e_export_format e)    { return e==FDBPackage::EX_RAW; };
        virtual FDBPackage::e_export_format DefaultFormat()    { return FDBPackage::EX_RAW; };

        virtual bool WriteRAW(const char*);
        virtual bool WriteINI(const char* filename);
        virtual bool WriteLUA(const char* filename);
        virtual bool WriteCSV(const char* filename);
        virtual bool WriteSQLITE3(const char* filename);

    protected:
        size_t  data_size;
        const uint8_t* data;
};

