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
            Redux    = 4 // SUBSTANCE REDUX
        };

#pragma pack(1)
        struct file_info
        {
            uint32_t total_size;
            uint32_t ftype;
            uint32_t compression;
            uint32_t size_uncomp;
            uint32_t size_comp;
            uint64_t mtime;
            uint32_t name_length;

            const char* name; 

            // for ftype==2:
            uint32_t comp_type;
            uint32_t width;
            uint32_t height;
            uint8_t  mipmapcount;
            uint8_t  unknown[3]; //garbage

            // calculated
            uint32_t rest_size;
        };
        static const size_t file_info_size_base = 6*sizeof(uint32_t)+sizeof(uint64_t);
        static const size_t file_info_size_image = 4*sizeof(uint32_t);
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
		uint8_t* GetFileRawData(size_t index, file_info& pinfo);
        int   GetFileData(size_t index, uint8_t* &uncomp_data, size_t& out_size, file_info* pinfo = NULL);

        size_t      FindFile(const char* fname);
        FDBFile*    GetFile(size_t index);
        bool        ExtractFile(size_t index, const char* filename, e_export_format format);
        uint32_t    CalcCRC32(size_t index);

    protected:
        FDBFile* CreateFileObject(const file_info&, uint8_t*);

    private:
        e_export_format TestFormat(FDBFile*, e_export_format);

#pragma pack(1)
        struct file_head_data
        {
            uint32_t ftype;
            uint64_t mtime;
            uint32_t offset;
        };
#pragma pack()

        FILE*   file;

        uint32_t  file_count;
        file_head_data* file_positions;
        uint32_t *      file_name_pos;
        const char*     file_names_data;
};

