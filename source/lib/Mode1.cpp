#include "stdafx.h"

// RLE
int Uncompress_Mode1(uint8_t *src, uint32_t src_size, uint8_t *dest)
{
	uint8_t *dest_start = dest;
	uint8_t *src_end = src+src_size;

  while ( src < src_end )
  {
	size_t length = (*src & 0x7F) + 1;
	bool is_const = (*src & 0x80) != 0;

	++src;
    if ( is_const )
    {
      memset(dest, *src++, length);
    }
    else
    {
      memcpy(dest, src, length);
      src += length;
    }

    dest += length;
  }
  return dest-dest_start;
}


