#include "stdafx.h"
#include "mode4.h"


struct internal_struct1
{
    internal_struct1* u1;    // 0
    DWORD u2a;   // 4
    DWORD u2b;   // 8
    DWORD u2c;   // 12
    BYTE* u3;    // 16 = 0x10
    DWORD u4;    // 20
    void* d1;    // 24  <-
    size_t  cur_size;    // 28
    size_t  page_size;    // 32 = 0x020
    size_t  total_size;    // 36
};

struct uncompress_data // size = 0x30
{
    uncompress_data* u1;  // (result of reduxHandleGetOutputDesc - )
    DWORD u2a; // (result of reduxHandleGetOutputDesc)
    DWORD u2b; // (result of reduxHandleGetOutputDesc)
    DWORD u2c; // (result of reduxHandleGetOutputDesc)
    BYTE* cur_data; // 0x10
    DWORD u12;
    void* d1;
    size_t  cur_size; 
    size_t  page_size;  //0x020
    size_t  total_size;
    char* filename;
    DWORD has_data; 
};

struct a1_struct
{
    uncompress_data* p_data; //0  <- == uncompress_data ??
    DWORD u1[3]; // 4
    DWORD u2[4]; // 16
    DWORD u3[3]; // 32
    DWORD flag1; // 44  BYTE <-
    void* dat1;  // 48       <-
    DWORD u4[3]; // 52
};


struct redux_status
{
    DWORD k1;
    DWORD k2;
    DWORD k3;
    size_t k4;
};

DWORD sub_55D0E0(BYTE a1);
size_t  calculate_image_size(signed int mipmap_count, int format, signed int width,signed int height);




HMODULE redux_dll=NULL;
bool   loading_error=false;
typedef int (__cdecl *t_reduxHandleDecompress)(void* data, int src_size, void* dst);
typedef int (*t_reduxCallbackSet)(unsigned nr, void* fct);
typedef int (*t_reduxHandleGetOutputDesc)(int a1, int a2, void* a3);
t_reduxHandleDecompress reduxHandleDecompress;
t_reduxCallbackSet reduxCallbackSet;
t_reduxHandleGetOutputDesc reduxHandleGetOutputDesc;

CRITICAL_SECTION CriticalSection; 




void __stdcall redux_callback1(int a1, a1_struct* a2, int a3);
void* __stdcall redux_callback2(int a1, a1_struct* a2, int a3, int a4, redux_status* a5);
void sub_41A220(void* a1);


HMODULE LoadLib()
{
	UINT oldmode = SetErrorMode(SEM_NOOPENFILEERRORBOX);

    redux_dll=LoadLibrary("redux_runtime.dll");
	if (!redux_dll)
	{
		std::string romdir = std::GetROMInstallDir();
		LoadLibrary( (romdir+"redux_nvtt.dll").c_str());
		redux_dll=LoadLibrary( (romdir+"redux_runtime.dll").c_str());

		if (!redux_dll)
		{
			MessageBox(NULL,"FDBex2 required additional DLL's.\n\nPlease copy these files from your RoM directory:\n -> 'redux_nvtt.dll'\n->'redux_runtime.dll'","DLL Missing",MB_OK);
		}
	}


    // TODO: inform user that he need to copy that stuff 'cause of copyrights
	SetErrorMode(oldmode);
	return redux_dll;
}

bool InitRedux()
{
    redux_dll=LoadLib();
    if (!redux_dll) return false;

    reduxHandleDecompress = (t_reduxHandleDecompress)GetProcAddress( redux_dll, "reduxHandleDecompress" );
    reduxCallbackSet = (t_reduxCallbackSet)GetProcAddress( redux_dll, "reduxCallbackSet" );
    reduxHandleGetOutputDesc = (t_reduxHandleGetOutputDesc)GetProcAddress( redux_dll, "reduxHandleGetOutputDesc" );
    
    if (!reduxHandleDecompress || 
        !reduxCallbackSet ||
        !reduxHandleGetOutputDesc)
    {
        ReleaseRedux();
        return false;
    }


    //     int __stdcall sub_55EBB0(int a1) // a2_struct 
    //     {
    //         *(_DWORD *)a1 = &off_96B22C;
    //         sub_560360(a1 + 264);
    //         sub_560520(a1 + 356);
    //         *(_DWORD *)(a1 + 452) = 0;
    //         *(_DWORD *)(a1 + 456) = 0;
    //         *(_DWORD *)(a1 + 460) = 0;
    //         *(_DWORD *)(a1 + 468) = 0;
    //         *(_DWORD *)(a1 + 472) = 0;
    //         *(_DWORD *)(a1 + 476) = 0;
    //         *(_BYTE *)(a1 + 480) = 0;
    //         memset((void *)(a1 + 4), 0, 0x104u);
    //         return a1;
    //     }
    reduxCallbackSet(6,redux_callback2);
    reduxCallbackSet(7,redux_callback1);


    InitializeCriticalSection(&CriticalSection);

    return true;
}

void ReleaseRedux()
{
    LeaveCriticalSection(&CriticalSection);

    if (redux_dll) FreeLibrary(redux_dll);
    redux_dll = NULL;
}

int Uncompress_Mode4(BYTE *src, unsigned int src_size, BYTE *dest, unsigned int dest_size)
{
    if (!redux_dll)
	{
		if (loading_error) return -1;
		if (!InitRedux())
		{
			loading_error = true;
			return -1;
		}
	}

    uncompress_data data;
    ZeroMemory(&data,sizeof(uncompress_data)); 
	// TODO: test if  "data.cur_data = dest" will work also

    // v3 = this -> FileHeader
    data.filename = "dummy";
    int res = reduxHandleDecompress(src,src_size,(void*)&data);
    if (res || data.cur_data==0) return -1;
    memcpy(dest,data.cur_data,data.cur_size);

	free(data.cur_data); 

    return 0;
}

void __stdcall redux_callback1(int a1, a1_struct* a2, int a3)
{
    uncompress_data* v4 = a2->p_data;
    void* v3 = v4->d1;
    v4->has_data = 1;

    if ( v3 )
    {
        EnterCriticalSection(&CriticalSection);
        sub_41A220(v3);
        LeaveCriticalSection(&CriticalSection);
        v4->d1 = 0;
    }
}

void sub_41A220(void* a1)
{
    // just guest:
    if ( a1 ) free(a1);
    
    // ori. code looks like a release with aligment

    /*
    int v2; // edx@4
    int v3; // ecx@4
    int v4; // ecx@6
    char v5; // zf@9
    int v6; // ecx@10

    if ( a1 )
    {
        unsigned int v1 = *(_DWORD *)(ptr_app_data + 4 * (a1 >> 24) + 520) + 32 * ((a1 >> 16) & 0xFF);
        if ( *(_DWORD *)(v1 + 16) == ptr_app_data + 508 )
        {
            VirtualFree((LPVOID)a1, 0, MEM_RELEASE);
        }
        else
        {
            if ( !*(_DWORD *)(v1 + 20) )
            {
                v3 = *(_DWORD *)(v1 + 24);
                v2 = v1 + 24;
                if ( v3 )
                    *(_DWORD *)(v3 + 28) = *(_DWORD *)(v1 + 28);
                **(_DWORD **)(v1 + 28) = *(_DWORD *)v2;
                v4 = *(_DWORD *)(v1 + 16);
                if ( *(_DWORD *)v4 )
                    *(_DWORD *)(*(_DWORD *)v4 + 28) = v2;
                *(_DWORD *)v2 = *(_DWORD *)v4;
                *(_DWORD *)(v1 + 28) = v4;
                *(_DWORD *)v4 = v1;
            }

            *(_DWORD *)(a1 + 4) = 1;
            *(_DWORD *)a1 = *(_DWORD *)(v1 + 20);
            v5 = *(_DWORD *)(v1 + 8)-- == 1;
            *(_DWORD *)(v1 + 20) = a1;
            if ( v5 )
            {
                v6 = *(_DWORD *)(v1 + 24);
                if ( v6 )
                    *(_DWORD *)(v6 + 28) = *(_DWORD *)(v1 + 28);

                **(_DWORD **)(v1 + 28) = *(_DWORD *)(v1 + 24);
                VirtualFree(*(LPVOID *)(v1 + 12), 0, MEM_RELEASE);
            }
        }
    }*/
}

inline WORD TopBit(WORD a)
{
    WORD b= (a>0)?1:0;
    while (b<a) b*=2;
    return b;
}


DWORD sub_55D0E0(BYTE a1);
size_t  calculate_image_size(signed int mipmap_count, int format, signed int width,signed int height);


// called on incoming data?
void* __stdcall redux_callback2(int a1, a1_struct* a2, int a3, int a4, redux_status* a5)
{
    uncompress_data* v5 = a2->p_data;

    v5->has_data = 1;

    if ( a4 == 0 )
    {
        struct res_struct
        {
            uncompress_data* new_data; // v16
            DWORD unk;   // v17
            union 
            {
                DWORD img_size;
                struct { WORD width; WORD height;};
            };
            union 
            {
                DWORD unk2;  // v19
                struct { BYTE mipmaps; BYTE format;  WORD uuu; };
            };

        } v16;

        reduxHandleGetOutputDesc(a1, a3, &v16); // a3=0;  a1=0x3a63e0
        v16.width = TopBit(v16.width);
        v16.height = TopBit(v16.height);

        v5->u1 = v16.new_data;
        v5->u2a = v16.unk;
        v5->u2b = v16.img_size;
        v5->u2c = v16.unk2;

        BYTE v8 = v16.format;
        DWORD edx = sub_55D0E0(v8);
        v5->cur_size = calculate_image_size(v16.mipmaps, edx, v16.width , v16.height);
        v5->cur_data = NULL;
        v5->d1 = NULL;
        v5->total_size = 0;

        if ( v5->cur_size )
        {
            v5->cur_data = (BYTE*)malloc(v5->cur_size);
            v5->page_size = a5->k4;
            return v5->cur_data;
        }

        v5->d1 = malloc( a5->k4 );
        v5->page_size = a5->k4;
        return v5->d1;
   }

    size_t v14 = v5->page_size;
    v5->total_size += v14;

    if ( v5->d1 == NULL && v5->total_size >= v5->cur_size && v14 )
        v5->d1 = malloc(v14);

    void* result = v5->d1;
    v5->page_size = a5->k4;
    if ( !result )
        result = v5->cur_data+v5->total_size;
    return result;
}


DWORD sub_55D0E0(BYTE a1)
{
  switch ( a1 )
  {
    case 0:
    case 4:     return 4;
    case 1:     return 5;
    case 8:     return 3;
    case 9:     return 7;
    case 16:
    case 20:    return 2;
    case 17:    return 8;
    case 21:    return 6;
    case 24:    return 1;
  }
  return 0;
}


size_t  calculate_image_size(signed int mipmap_count, int format, signed int width,signed int height)
{
    signed int v4; // ebx@1
    signed int v5; // edi@1
    int v7; // edx@4
    signed int v9; // ecx@6
    signed int v10; // eax@8
    signed int v11; // ecx@11
    signed int v12; // ecx@11


    size_t res = 0;

    v5 = width;
    v4 = height;

    if ( mipmap_count <= 1 )   mipmap_count = 1;

    v7 = format - 1;

    for (;mipmap_count>0;--mipmap_count)
    {

      switch ( v7 )
      {
        case 0:
        case 1:
                v9 = (v5<=1)?1:v5;
                v10 = (v4<=1)?1:v4;
                res += 2 * v9 * v10;
                break;
        case 2:
                v9 = (v5<=1)?1:v5;
                v10 = (v4<=1)?1:v4;
                res += v9 * v10 + 2 * v9 * v10;
                break;
        case 3:
                v9 = (v5<=1)?1:v5;
                v10 = (v4<=1)?1:v4;
                res += 4 * v9 * v10;
                break;
        case 4:
        case 5: // DDS
                v11 = (v5<=4)?4:v5;
                v12 = (v4<=4)?4:v4;
                res += v11 * v12 / 2;
                break;
        case 6:
        case 7:
                v11 = (v5<=4)?4:v5;
                v12 = (v4<=4)?4:v4;
                res += v11 * v12;
                break;
      }
      v5 >>= 1;
      v4 >>= 1;
    }

    return res;
}


