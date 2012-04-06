#pragma once

#include <stdio.h>

class FDBFile;

class FDBPackage
{
    public:
        enum e_export_format
        {
            EX_NONE = 0,
            EX_RAW = 1,
            EX_INI = 2,
            EX_LUA = 4,
            EX_CSV = 8,
            EX_SQLITE3 = 16,
        };

    public:
        enum file_compression
        {
            unpacked = 0,
            RLE      = 1,
            LZO      = 2,
            Inflate  = 3,
            nvTT     = 4
        };

#pragma pack(1)
        struct file_info
        {
            UINT32 total_size;
            UINT32 ftype;
            UINT32 compression;
            UINT32 size_uncomp;
            UINT32 size_comp;
            UINT64 mtime;
            UINT32 name_length;

            const char* name; 

            // for ftype==2:
            UINT32 comp_type; 
            UINT32 width;
            UINT32 height;
            UINT8  mipmapcount;
            UINT8  unknown[3]; //garbage

            // calculated
            UINT32 rest_size;
        };
        static const size_t file_info_size_base = 6*sizeof(UINT32)+sizeof(UINT64);
        static const size_t file_info_size_image = 4*sizeof(UINT32);
#pragma pack()


    public:
        FDBPackage(void);
        FDBPackage(const char*);
        ~FDBPackage(void);

        bool Open(const char*);
        void Close();
        bool IsOk();

        size_t  GetFileCount();
        const char* GetFileName(size_t index);
        void  GetFileInfo(size_t index, file_info&);
        int   GetFileData(size_t index, BYTE* &uncomp_data, size_t& out_size, file_info* pinfo = NULL);

        size_t      FindFile(const char* fname);
        FDBFile*    GetFile(size_t index);
        bool        ExtractFile(size_t index, const char* filename, e_export_format format);
        DWORD       CalcCRC32(size_t index);

    protected:
        FDBFile* CreateFileObject(const file_info&, BYTE*);

    private:
        e_export_format TestFormat(FDBFile*, e_export_format);

#pragma pack(1)
        struct file_head_data
        {
            UINT32 ftype;
            UINT64 mtime;
            UINT32 offset;
        };
#pragma pack()

        FILE*   file;

        UINT32  file_count;
        file_head_data* file_positions;
        UINT32 *        file_name_pos;
        const char*     file_names_data;



};

