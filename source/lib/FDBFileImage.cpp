#include "stdafx.h"

#include "FDBFile.h"
#include "dds.h"



FDBFileImage::FDBFileImage(const FDBPackage::file_info& s_info, BYTE* data ) 
    : FDBFile(s_info,data)
{
    width = s_info.width;
    height = s_info.height;
}




FDBFileImageTGA::FDBFileImageTGA(const FDBPackage::file_info& s_info, BYTE* data )
    : FDBFileImage(s_info,data)
{
}

bool FDBFileImageTGA::WriteRAW(const char* filename)
{
    #pragma pack(1)
    struct
    {
        BYTE  identsize;         // size of ID field that follows 18 byte header (0 usually)
        BYTE  colourmaptype;     // type of colour map 0=none, 1=has palette
        BYTE  imagetype;         // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

        WORD colourmapstart;     // first colour map entry in palette
        WORD colourmaplength;    // number of colours in palette
        BYTE  colourmapbits;     // number of bits per palette entry 15,16,24,32

        WORD xstart;             // image x origin
        WORD ystart;             // image y origin

        WORD width;              // image width in pixels
        WORD height;             // image height in pixels
        BYTE  bits;              // image bits per pixel 8,16,24,32
        BYTE  descriptor;        // image descriptor bits (vh flip bits)
    } TGA_HEADER = 
    {
        0,0,2, 0,0,0, 0,0,
        width,height,0x20,0x20
    };
    #pragma pack()

    FILE* outf;
    if (fopen_s(&outf,filename,"wb")) return false;
	if (!outf) return false;

    fwrite(&TGA_HEADER,sizeof(TGA_HEADER),1,outf);

    size_t res = fwrite(data,data_size,1,outf);
    fclose(outf);

    return res==1;
}

FDBFileImageDDS::FDBFileImageDDS(const FDBPackage::file_info& s_info, BYTE* data )
    : FDBFileImage(s_info,data)
{
    mipmapcount = s_info.mipmapcount;
    comp_type = s_info.comp_type;
}


bool FDBFileImageDDS::WriteRAW(const char* filename)
{
    DDS_HEADER head;
    ZeroMemory(&head,sizeof(DDS_HEADER));

    head.dwSize = sizeof(DDS_HEADER);
    head.dwHeight = height;
    head.dwWidth = width;
    head.dwMipMapCount = mipmapcount;
    head.dwFlags = DDS_HEADER_FLAGS_TEXTURE | DDS_HEADER_FLAGS_LINEARSIZE;
    head.dwCaps = DDS_SURFACE_FLAGS_TEXTURE;

    switch (comp_type)
    {
        case 4: head.ddpf= DDSPF_A8R8G8B8; break;
        case 5: head.ddpf= DDSPF_DXT1; break;
        case 6: head.ddpf= DDSPF_DXT1; head.ddpf.dwFlags |=1; break; // + Alpha
        case 8: head.ddpf= DDSPF_DXT5; break;

        default:
            assert(false);
    }


    if (mipmapcount > 1)
    {
        head.dwFlags |= DDS_HEADER_FLAGS_MIPMAP;
        head.dwCaps |= DDS_SURFACE_FLAGS_MIPMAP;
    }

    head.dwLinearSize = width * 32;
    if (comp_type == 8) head.dwLinearSize = width*64*8;


    // write
    FILE* outf;
    if (fopen_s(&outf,filename,"wb")) return false;
	if (!outf) return false;

    DWORD magic = 0x20534444;
    fwrite(&magic,sizeof(DWORD),1,outf);
    fwrite(&head,sizeof(head),1,outf);
    size_t res = fwrite(data,data_size,1,outf);
    fclose(outf);

    return res==1;
}


FDBFileImageBMP::FDBFileImageBMP(const FDBPackage::file_info& s_info, BYTE* data )
    : FDBFileImage(s_info,data)
{
    byte_count = s_info.comp_type;
    assert(byte_count==4); // not tested with other formats
}

bool FDBFileImageBMP::WriteRAW(const char* filename)
{
    // write bitmap
    BITMAPFILEHEADER bmfh;
    bmfh.bfType = 'MB'; 
    bmfh.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+width*height*byte_count;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
    BITMAPINFOHEADER bminfo;
    bminfo.biSize = sizeof(BITMAPINFOHEADER); 
    bminfo.biWidth = width; 
    bminfo.biHeight = -(LONG)height; 
    bminfo.biPlanes = 1; 
    bminfo.biBitCount = byte_count*8; 
    bminfo.biCompression = BI_RGB; 
    bminfo.biSizeImage = 0; 
    bminfo.biXPelsPerMeter = width; 
    bminfo.biYPelsPerMeter = height; 
    bminfo.biClrUsed = 0; 
    bminfo.biClrImportant = 0; 

    FILE* outf;
    if (fopen_s(&outf,filename,"wb")) return false;
	if (!outf) return false;

    fwrite(&bmfh,sizeof(BITMAPFILEHEADER),1,outf);
    fwrite(&bminfo,sizeof(BITMAPINFOHEADER),1,outf);

    size_t res = fwrite(data,data_size,1,outf);
    fclose(outf);

    return res==1;
}


FDBFileImagePNG::FDBFileImagePNG(const FDBPackage::file_info& s_info, BYTE* data )
    : FDBFileImage(s_info,data)
{
}

bool FDBFileImagePNG::WriteRAW(const char* filename)
{
    return FDBFileImage::WriteRAW(filename);
    /*
    BYTE magic[8] = {137,80,78,71,13,10,26,10};

    FILE* outf;
    if (fopen_s(&outf,filename,"wb")) return false;

    fwrite(magic,8,1,outf);

    // TODO: write chunks
    size_t res = fwrite(data,data_size,1,outf);
    fclose(outf);

    return res==1;
    */
}