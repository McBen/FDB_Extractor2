//#include "stdafx.h"
#if 0
#include "../cmd/commandline.h"

#include <cfixcc.h>

class CommandlineTest : public cfixcc::TestFixture , CommandLine
{
    public:
        void TestCommandline() 
        {
            const char* argv1[]={"fdb_ex2.exe","-o","temp","t?est.b*"};
            int argc1 = sizeof(argv1)/sizeof(argv1[0]);
            Parse(argc1, argv1);

            CFIXCC_ASSERT_EQUALS( "temp/", output_directory.c_str()  );
            CFIXCC_ASSERT_EQUALS( "t.?est\\.b.*", filter.str().c_str()  );

            const char* argv2[]={"fdb_ex2.exe",".tga","e:\\tmp.fdb"};
            int argc2 = sizeof(argv2)/sizeof(argv2[0]);
            Parse(argc2, argv2);

            CFIXCC_ASSERT_EQUALS( true, output_directory.empty()  );
            CFIXCC_ASSERT_EQUALS( "\\.tga", filter.str().c_str()  );
            CFIXCC_ASSERT_EQUALS( "e:\\tmp.fdb", fbd_files[0].c_str() );

            const char* argv3[]={"fdb_ex2.exe","-l","-s","-r","$data\\\\","\"c:\\t et\\temp\"","c:\\tmp.fdb"};
            int argc3 = sizeof(argv3)/sizeof(argv3[0]);
            Parse(argc3, argv3);

            CFIXCC_ASSERT_EQUALS( true, list_only );
            CFIXCC_ASSERT_EQUALS( false, verbose );
            CFIXCC_ASSERT_EQUALS( true, db_sql_out );
            CFIXCC_ASSERT_EQUALS( true, output_directory.empty()  );
            CFIXCC_ASSERT_EQUALS( "$data\\\\", filter.str().c_str()  );

            CFIXCC_ASSERT_EQUALS( "\"c:\\t et\\temp\"", fbd_files[0].c_str() );
            CFIXCC_ASSERT_EQUALS( "c:\\tmp.fdb", fbd_files[1].c_str() );
        }


        void TestRegistry()
        {
            #if WIN32
                std::string instdir = GetROMInstallDir();
                CFIXCC_ASSERT_EQUALS( "C:\\Program Files\\Runes of Magic\\", instdir.c_str() );
            #endif
        }
};

CFIXCC_BEGIN_CLASS( CommandlineTest )
    CFIXCC_METHOD( TestCommandline )
CFIXCC_END_CLASS()

#endif