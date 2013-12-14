#include "utils.h"
#include "commandline.h"
#include "../VersionNo.h"

#include <iostream>
#include "boost/program_options.hpp"
using namespace std;
using namespace boost::program_options;


CommandLine::CommandLine() :
	options(0)
{
}

bool CommandLine::Parse(int argc, const char* argv[])
{
    options_description generic_options("Options:");
    generic_options.add_options()
        ("help,?", "help message")
        ("verbose,v", "detailed output")
        ("overwrite,y", "always overwrite files")
        ("list,l", "list files only (no extraction)")
        ("list_crc,x", "list files with checksum (no extraction)")
        ("list_crcraw,X", "list files with checksum (no extraction,no decompress=>quick)")
        ("list_full,u", "list files with details (no extraction)")
        ("regex,r", "use regex filter instead of filename")
        ("output,o", value<string>(&output_directory)->default_value(""), "output directory")
        ("fdb_file,i", value< vector<string> >(&fbd_files),"FDB file")
        ("filename", value<string>()->required(), "file/filter to extract\nyou can use a dos-like filter or (if '-r') a regular expression")
        ("sql,s", "generate SQL-Scripts for DB files")
        ("csv,c", "generate CSV-Files for DB files")

        ("raw,p", "write unconverted raw-data (for developers)")
        ("lua,a", "write lua")
        ;

    options_description hidden_options("Hidden:");
    hidden_options.add_options()
//         ("raw,p", "write unconverted raw-data (for developers)")
//         ("lua,a", "write lua")
        ;

    options_description cmdline_options;
    cmdline_options.add(generic_options).add(hidden_options);

    positional_options_description positional_opts;
    positional_opts.add("filename", 1).add("fdb_file", -1);

	variables_map vm;
    try
    {
		parsed_options parsed = command_line_parser(argc, argv)
			.options(cmdline_options)
			.positional(positional_opts)
			//.allow_unregistered()
			.run();

		store(parsed, vm);


		if (vm.count("help") || vm.size()==0) {
			cout << "fdbex " << STRFILEVER  <<"\n";
			cout << "Usage: [options] filename fdb_file\n\n";
			cout << generic_options << "\n";
			return false;
		}

        notify(vm);

		ParseArguments(vm);

    } catch(boost::program_options::error& e)
    {
        cout << "Usage: [options] filename fdb_file\n\n";
        cout << generic_options << "\n";
        return false;
    } catch(boost::regex_error& e)
    {
        cerr << "not a valid regular expression: \"" << e.what() << "\"" << endl;
        return false;
    }

    return true;
}

void CommandLine::ParseArguments(variables_map vm)
{
    options = 
        (vm.count("verbose")>0? OPT_VERBOSE : 0) | 
        (vm.count("overwrite")>0? OPT_OVERWRITE : 0) | 

        (vm.count("list")>0? OPT_LIST_ONLY : 0) | 
        (vm.count("list_crc")>0? OPT_LIST_ONLY_WITH_CRC : 0) | 
        (vm.count("list_crcraw")>0? OPT_LIST_ONLY_WITH_CRCRAW : 0) | 
        (vm.count("list_full")>0? OPT_LIST_ONLY_FULL : 0) | 

        (vm.count("raw")>0? OPT_RAW_DATA : 0) | 
        (vm.count("sql")>0? OPT_DB_SQL_OUT : 0) | 
        (vm.count("lua")>0? OPT_LUA_OUT : 0) | 
        (vm.count("csv")>0? OPT_CSV_OUT : 0);

    if (fbd_files.size()==0)
    {
        string instdir = GetROMInstallDir();
        if (!instdir.empty())
        {
            fbd_files.push_back(instdir+"fdb/*.fdb");
            cout << "using RoM-Dir: " << instdir<<"\n";
        } 
        else
        {
            cerr << "no FDB File set\n";
			#if BOOST_VERSION<105000
	            boost::throw_exception(invalid_command_line_syntax("fdb_file", invalid_command_line_syntax::missing_parameter)); 
			#else
		        boost::throw_exception(invalid_command_line_syntax(invalid_command_line_syntax::missing_parameter,"fdb_file")); 
			#endif
        }
    }

    if (!output_directory.empty())
    {
        if (*(output_directory.end()-1)!='\\' && *(output_directory.end()-1)!='/')  output_directory += "/";
    }

    string in_filter = vm["filename"].as<string>(); 

    if (vm.count("regex")==0)
    {
        in_filter = ReplaceString(in_filter,"/","\\"); // RoM uses backslashes only

        // TODO: create a 'escape_regex' function
        in_filter = ReplaceString(in_filter,"\\","\\\\");

        in_filter = ReplaceString(in_filter,"^","\\^");
        if (in_filter[0]=='\\') in_filter[0]='^';

        in_filter = ReplaceString(in_filter,".","\\.");
        in_filter = ReplaceString(in_filter,"$","\\$");
        in_filter = ReplaceString(in_filter,"*",".*");
        in_filter = ReplaceString(in_filter,"?",".?");
    }

    filter.assign(in_filter, boost::regex_constants::icase);

}


void CommandLine::HelpOut()
{
    const char* argv[]={"","-?"};
    Parse(2,argv);
}

