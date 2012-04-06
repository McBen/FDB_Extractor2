#include "stdafx.h"
#include "lzo\lzo1x.h"

int Uncompress_Mode2(BYTE *src, unsigned int src_size, BYTE *dest, unsigned int& ptr_dest_size)
{
	lzo_uint temp = ptr_dest_size;
	return lzo1x_decompress( src, src_size,dest,&temp,NULL);
}
