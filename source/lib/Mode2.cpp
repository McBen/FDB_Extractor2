#include "stdafx.h"
#include "lzo/lzo1x.h"

int Uncompress_Mode2(uint8_t *src, uint32_t src_size, uint8_t *dest, uint32_t& ptr_dest_size)
{
	lzo_uint temp = ptr_dest_size;
	return lzo1x_decompress( src, src_size,dest,&temp,NULL);
}
