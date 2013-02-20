#pragma once


class FDBFileImage : public FDBFile
{
    public:
        FDBFileImage(const FDBPackage::file_info& s_info, uint8_t* data );

    protected:
        unsigned width;
        unsigned height;
};


class FDBFileImageTGA : public FDBFileImage
{
    public:
        FDBFileImageTGA(const FDBPackage::file_info& s_info, uint8_t* data );

        bool WriteRAW(const char*);
};

class FDBFileImageDDS : public FDBFileImage
{
    public:
        FDBFileImageDDS(const FDBPackage::file_info& s_info, uint8_t* data );

        bool WriteRAW(const char*);

    protected:
        unsigned mipmapcount;
        unsigned comp_type;
};

class FDBFileImageBMP : public FDBFileImage
{
    public:
        FDBFileImageBMP(const FDBPackage::file_info& s_info, uint8_t* data );

        bool WriteRAW(const char*);
    protected:
        unsigned byte_count;
};

class FDBFileImagePNG : public FDBFileImage
{
    public:
        FDBFileImagePNG(const FDBPackage::file_info& s_info, uint8_t* data );

        bool WriteRAW(const char*);
};