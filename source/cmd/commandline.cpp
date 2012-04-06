#include <Windows.h>

#include "utils.h"
#include "commandline.h"

#include <iostream>
#include "boost/program_options.hpp"
using namespace std;
using namespace boost::program_options;


CommandLine::CommandLine()
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
    verbose = vm.count("verbose")>0;
    overwrite = vm.count("overwrite")>0;
    neveroverwrite = false;
    list_only = vm.count("list")>0;
	list_only_with_crc = vm.count("list_crc")>0;
    raw_data = vm.count("raw")>0;
    db_sql_out = vm.count("sql")>0;
    lua_out = vm.count("lua")>0;
    csv_out = vm.count("csv")>0;

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
            boost::throw_exception(invalid_command_line_syntax("fdb_file", invalid_command_line_syntax::missing_parameter)); 
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

