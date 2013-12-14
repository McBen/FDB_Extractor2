#pragma once
#include <boost/program_options.hpp>
#include <boost/regex.hpp>

class CommandLine
{
    public:
        enum E_OPTIONS
        {
            OPT_VERBOSE         = 0x0001,
            OPT_OVERWRITE       = 0x0002,
            OPT_NEVEROVERWRITE  = 0x0004,
            OPT_LIST_ONLY               = 0x0010,
		    OPT_LIST_ONLY_FULL          = 0x0020,
            OPT_LIST_ONLY_WITH_CRC      = 0x0040,
            OPT_LIST_ONLY_WITH_CRCRAW   = 0x0080,
            OPT_ANY_LIST = OPT_LIST_ONLY | OPT_LIST_ONLY_FULL | OPT_LIST_ONLY_WITH_CRC | OPT_LIST_ONLY_WITH_CRCRAW,
            OPT_RAW_DATA    = 0x0100,
            OPT_DB_SQL_OUT  = 0x0200,
            OPT_LUA_OUT     = 0x0400,
            OPT_CSV_OUT     = 0x0800,
        };

    public:
        std::vector<std::string> fbd_files;
        std::string output_directory;

        uint16_t options;

        boost::regex filter;

    public:
        CommandLine();

        bool Parse(int argc, const char* argv[]);

        void HelpOut();

    private:

        void ParseArguments(boost::program_options::variables_map vm);
};

