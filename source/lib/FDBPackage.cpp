#include "stdafx.h"

#include "decrypte.h"

#include <assert.h>
#include <string.h>

#include <string>
using namespace std;

#include <boost/crc.hpp>


// TODO: use memory blocks instead of dynamic alloc 
class MemoryBlock
{
    private:
        size_t  alloc_size;
        uint8_t*   data;
        bool    is_used;

    public:
        MemoryBlock(size_t min_size=0x1000)
        {
            is_used= false;
            alloc_size=min_size;
            data = (uint8_t*)malloc(min_size);
        }

        ~MemoryBlock()
        {
            assert(!is_used);
            free(data);
        }

        void Release()
        {
            assert(is_used);
            is_used= false;
        }

        uint8_t* GetData(size_t size)
        {
            assert(!is_used);

            if (size>alloc_size)
            {
                free(data);
                alloc_size=size;
                data = (uint8_t*)malloc(size);
            }
            is_used = true;
            return data;
        }

        
};

MemoryBlock b1,b2;


FDBPackage::FDBPackage(void)
{
    file = NULL;
    file_positions = NULL;
    file_names_data = NULL;
    file_name_pos = NULL;
}

FDBPackage::FDBPackage(const char* filename)
{
    file = NULL;
    file_positions = NULL;
    file_names_data = NULL;
    file_name_pos = NULL;
    Open(filename);
}

FDBPackage::~FDBPackage(void)
{
    Close();
}

void FDBPackage::Close()
{
    if (file) fclose(file);

    delete [] (file_positions);
    delete [] (file_names_data);
    delete [] (file_name_pos);

    file = NULL;
    file_positions = NULL;
    file_names_data = NULL;
    file_name_pos = NULL;
}


bool FDBPackage::Open(const char* filename)
{
    Close();

    if (fopen_s(&file,filename,"rb"))
    {
        file = NULL;
        return false;
    }

    // intro
    uint32_t magic;
    fread(&magic,sizeof(uint32_t),1,file);
    fread(&file_count,sizeof(uint32_t),1,file);

    if (magic != 0x46444201) 
    {
        Close();
        return false;
    }

    // position
    file_positions = new file_head_data[file_count];
    fread(file_positions,sizeof(file_head_data),file_count,file);

    // names
    file_name_pos = new uint32_t [file_count];
    fread(file_name_pos,sizeof(uint32_t),file_count,file);
    int pos=0;
    for (uint32_t t=0;t<file_count;++t)
    {
        int len = file_name_pos[t];
        file_name_pos[t] = pos;
        pos +=len+1;
    }

    uint32_t data_size;
    fread(&data_size,sizeof(uint32_t),1,file);
    file_names_data = new char[data_size];
    fread((void*)file_names_data,data_size,1,file);

    return true;
}

bool FDBPackage::IsOk()
{
    return file!=NULL;
}

size_t  FDBPackage::GetFileCount()
{
    assert(IsOk());

    return file_count;
}

const char* FDBPackage::GetFileName(size_t index)
{
    assert(index < file_count);
    assert(IsOk());

    return file_names_data+file_name_pos[index];
}

void    FDBPackage::GetFileInfo(size_t index, file_info& s_info)
{
    assert(index < file_count);
    assert(IsOk());

    fseek(file, file_positions[index].offset,SEEK_SET);
    fread(&s_info,file_info_size_base,1,file);

    s_info.name = file_names_data+file_name_pos[index];
    s_info.rest_size = s_info.total_size - file_info_size_base - s_info.name_length;

	#ifdef _DEBUG
		char* temp_name = new char[s_info.name_length];
		fread(temp_name, s_info.name_length,1,file);
		assert(strcmp(temp_name,s_info.name)==0);
		assert(file_positions[index].ftype == s_info.ftype);
		assert(file_positions[index].mtime == s_info.mtime);
		delete[] (temp_name);
	#else
		fseek(file,s_info.name_length,SEEK_CUR);
	#endif

    if (s_info.ftype==2)
    {
        fread(&s_info.comp_type, file_info_size_image, 1, file);
        s_info.rest_size -= file_info_size_image;
    }
}

uint8_t*   FDBPackage::GetFileRawData(size_t index, file_info& sinfo)
{
    assert(index < file_count);
    assert(IsOk());

    GetFileInfo(index, sinfo);

    uint8_t* rawdata = (uint8_t*)malloc(sinfo.rest_size);
	if (!rawdata) return NULL;
    fread(rawdata, sinfo.rest_size,1,file);

	return rawdata;
}

int   FDBPackage::GetFileData(size_t index, uint8_t* &uncomp_data, size_t& out_size, file_info* pinfo /*=NULL*/)
{
    file_info s_info;
    if (!pinfo) pinfo=&s_info;

    uint8_t* rawdata = GetFileRawData(index, *pinfo);

    out_size = pinfo->size_uncomp;
    uint32_t result_size;
    int res;

    switch (pinfo->compression)
    {
        case unpacked:
            assert(pinfo->size_comp==0); 
            assert(pinfo->size_uncomp==pinfo->rest_size); 
            uncomp_data = rawdata;
            return 0;

        case RLE:
            assert(pinfo->size_comp==pinfo->rest_size); 

            uncomp_data = new uint8_t[ pinfo->size_uncomp ];
            result_size = Uncompress_Mode1(rawdata, pinfo->size_comp, uncomp_data);

            assert( result_size == pinfo->size_uncomp ); 
            free (rawdata);
            return 0;

        case LZO:
            assert(pinfo->size_comp==pinfo->rest_size); 

            uncomp_data = new uint8_t[ pinfo->size_uncomp ];
			result_size=pinfo->size_uncomp;
            Uncompress_Mode2(rawdata, pinfo->size_comp, uncomp_data, result_size);

            assert( result_size == pinfo->size_uncomp ); 
            free (rawdata);
            return 0;

        case Inflate:
            assert(pinfo->size_comp==pinfo->rest_size); 

            uncomp_data = new uint8_t[ pinfo->size_uncomp ];
            result_size = pinfo->size_uncomp;
            res = Uncompress_Mode3(rawdata, pinfo->size_comp, uncomp_data, &result_size);

            assert( result_size == pinfo->size_uncomp && (res==0 || (res==-3 && result_size==1)) );  //TODO: inspect strange case: size=1
            free (rawdata);
            return 0;

        case Redux:
            assert(pinfo->size_comp==pinfo->rest_size); 

            uncomp_data = new uint8_t[ pinfo->size_uncomp ];
            res = Uncompress_Mode4(rawdata, pinfo->size_comp, uncomp_data, pinfo->size_uncomp);

            assert(res==0);
            free (rawdata);
            return 0;

        default:
            assert(false);
            break;
    }

    free (rawdata);
    return -1;
}

size_t      FDBPackage::FindFile(const char* fname)
{
    assert(IsOk());

    uint32_t* name_pos = file_name_pos;
    for (size_t t=0;t<file_count;++t,++name_pos)
        if (strcmp(file_names_data+(*name_pos), fname) ==0) return t;

    return -1;
}

bool    FDBPackage::ExtractFile(size_t index, const char* filename, e_export_format format)
{
    FDBFile* file = GetFile(index);
    if (!file) return false;

    format = TestFormat(file,format);

    bool res= false;
    if ( format & EX_RAW) res|= file->WriteToFile(filename,EX_RAW);
    if ( format & EX_CSV) res|= file->WriteToFile(filename,EX_CSV);
    if ( format & EX_INI) res|= file->WriteToFile(filename,EX_INI);
    if ( format & EX_LUA) res|= file->WriteToFile(filename,EX_LUA);
    if ( format & EX_SQLITE3) res|= file->WriteToFile(filename,EX_SQLITE3);

    delete(file);
    return res;
}

 FDBPackage::e_export_format FDBPackage::TestFormat(FDBFile* f, e_export_format e)
 {
    if ( e & EX_RAW) if (!f->ExportFormatIsValid(EX_RAW)) e= (e_export_format) (e & ~EX_RAW);
    if ( e & EX_CSV) if (!f->ExportFormatIsValid(EX_CSV)) e= (e_export_format) (e & ~EX_CSV);
    if ( e & EX_INI) if (!f->ExportFormatIsValid(EX_INI)) e= (e_export_format) (e & ~EX_INI);
    if ( e & EX_LUA) if (!f->ExportFormatIsValid(EX_LUA)) e= (e_export_format) (e & ~EX_LUA);
    if ( e & EX_SQLITE3) if (!f->ExportFormatIsValid(EX_SQLITE3)) e= (e_export_format) (e & ~EX_SQLITE3);

    if (e == EX_NONE) e= f->DefaultFormat();

    return e;
 }

 uint32_t  FDBPackage::CalcCRC32(size_t index)
{
    file_info s_info;

    uint8_t* data=GetFileRawData(index, s_info);

	boost::crc_32_type  crc;
	crc.process_bytes( data, s_info.rest_size );
	free(data);

	return  crc.checksum();
}

FDBFile*    FDBPackage::GetFile(size_t index)
{
    file_info s_info;
    uint8_t* data;
    size_t data_size;

    if (GetFileData(index, data, data_size, &s_info))
        return NULL;

    FDBFile* file = CreateFileObject(s_info,data);
    return file;
}

FDBFile* FDBPackage::CreateFileObject(const file_info& s_info, uint8_t* data)
{
    boost::filesystem::path filepath(s_info.name);
    
    string file_ext = filepath.extension().generic_string();
    transform(file_ext.begin(), file_ext.end(), file_ext.begin(), ::tolower);

    if (s_info.ftype == 2)
    {
        if (file_ext == ".tga")
        {
            return new FDBFileImageTGA(s_info,data);
        }
        if (file_ext == ".dds")
        {
            return new FDBFileImageDDS(s_info,data);
        }
        if (file_ext == ".bmp")
        {
            return new FDBFileImageBMP(s_info,data);
        }

        if (file_ext == ".png")
        {
            return new FDBFileImagePNG(s_info,data);
        }

        assert(false);
        delete (data);
        return NULL;
    }


    if (file_ext==".db")
    {
        string filename = filepath.filename().generic_string();
        transform(filename.begin(), filename.end(), filename.begin(), ::tolower);

        // specials
        if (filename=="zonechannel.db" || filename=="thumbs.db") 
                return new FDBFile(s_info,data);

        // strings
        if (filename.compare(0, 6, "string") == 0)
        {
            return new FDBFileStringDB(s_info,data);
        }

		if (filename=="learnmagic.db") 
			return new FDBFileDB_LearnMagic(s_info,data);

        return new FDBFileDB(s_info,data);
    }


    return new FDBFile(s_info,data);
}
