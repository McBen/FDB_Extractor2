#include "stdafx.h"
#include "FDBFile.h"

FDBFile::FDBFile(const FDBPackage::file_info& s_info, uint8_t* _data )
{
    data_size = s_info.size_uncomp;
    data = _data;
}

FDBFile::~FDBFile()
{
    delete(data);
}

std::string FDBFile::GetTargetName(const char* filename, FDBPackage::e_export_format e)
{
    if (e==FDBPackage::EX_NONE) e = DefaultFormat();
    assert(ExportFormatIsValid(e));

    switch (e)
    {
        case FDBPackage::EX_CSV:
            return std::string(filename)+".csv";

        case FDBPackage::EX_SQLITE3:
            return std::string(filename)+".sql";
    }

    return filename;
}

bool FDBFile::WriteToFile(const char* filename, FDBPackage::e_export_format e)
{
    if (e==FDBPackage::EX_NONE) e = DefaultFormat();

    assert(ExportFormatIsValid(e));

    bool res=false;
    switch (e)
    {
        case FDBPackage::EX_RAW:
            res = WriteRAW(filename);
            break;

        case FDBPackage::EX_INI:
            res = WriteINI(filename);
            break;

        case FDBPackage::EX_LUA:
            res = WriteLUA(filename);
            break;

        case FDBPackage::EX_CSV:
            res = WriteCSV( GetTargetName( filename, e).c_str());
            break;

        case FDBPackage::EX_SQLITE3:
            res = WriteSQLITE3(GetTargetName( filename, e).c_str());
            break;
    }

    return res;
}


bool FDBFile::WriteRAW(const char* filename)
{
    FILE* outf;
    if (fopen_s(&outf,filename,"wb")) return false;
	if (!outf) return false;

    size_t res = fwrite(data,data_size,1,outf);
    fclose(outf);

    return res==1;
}

bool FDBFile::WriteINI(const char* filename)        
{ 
    return WriteRAW(filename);
}

bool FDBFile::WriteLUA(const char* filename)
{ 
    return WriteRAW(filename);
}

bool FDBFile::WriteCSV(const char* filename)        
{
    return WriteRAW(filename);
}

bool FDBFile::WriteSQLITE3(const char* filename) 
{ 
    return WriteRAW(filename);
}

