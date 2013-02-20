#include "stdafx.h"

extern "C"
{
    #include "zlib.h"
};


int Uncompress_Mode3(uint8_t *src, uint32_t src_size, uint8_t *dest,uint32_t* dest_size)
{

    z_stream strm;

    strm.zalloc = (alloc_func)0;
    strm.zfree = (free_func)0; 
    strm.avail_in = src_size;
    strm.next_in = src; 
    strm.avail_out = *dest_size; 
    strm.next_out = dest; 


    int res = inflateInit(&strm);
    if (res!=Z_OK) 
        return res;

    res = inflate(&strm, Z_NO_FLUSH);// Z_FINISH);
    if (res!=Z_OK) 
    {
        inflateEnd(&strm); 
        return res;
    }

    res = inflateEnd(&strm); 
    if (res==Z_OK && (strm.avail_in!=0 || strm.avail_out!=0)) 
    {
        res = Z_BUF_ERROR;
    }

    *dest_size = *dest_size - strm.avail_out;

    return res;
}
