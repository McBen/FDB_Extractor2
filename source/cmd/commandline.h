#pragma once
#include <boost/program_options.hpp>
#include <boost/regex.hpp>

class CommandLine
{
    public:
        std::vector<std::string> fbd_files;
        std::string output_directory;
        bool verbose;
        bool overwrite;
        bool neveroverwrite;
        bool list_only;
        bool list_only_with_crc;
		bool list_only_full;
        bool raw_data; 
        bool db_sql_out; 
        bool lua_out; 
        bool csv_out; 

        boost::regex filter;

    public:
        CommandLine();

        bool Parse(int argc, const char* argv[]);

        void HelpOut();

    private:

        void ParseArguments(boost::program_options::variables_map vm);
};

