#include "stdafx.h"


// RLE
int Uncompress_Mode1(uint8_t *src, uint32_t src_size, uint8_t *dest)
{
  int v3; // ebx@1
  unsigned int v4; // esi@1
  size_t v5; // edi@2
  int v6; // esi@2
  char v7; // al@2
  int v8; // ST04_4@3

  v3 = 0;
  v4 = 0;
  while ( v4 < src_size )
  {
    v7 = src[v4] & 0x80;
    v5 = (src[v4] & 0x7F) + 1;
    v6 = v4 + 1;
    if ( v7 )
    {
      v8 = src[v6];
      v4 = v6 + 1;
      memset(dest+v3, v8, v5);
      v3 += v5;
    }
    else
    {
      memcpy(dest+v3, v6 + src, v5);
      v3 += v5;
      v4 = v5 + v6;
    }
  }
  return v3;
}

