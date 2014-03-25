#include <stdint.h>
#include "commandline.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "FDB_LIB.h"

CommandLine cmdline;


using namespace std;

bool Process();
bool List(uint16_t options);

int main( int argc, const char* argv[] )
{
     try
    {
        if (!cmdline.Parse(argc, argv))  return -1;
    }
    catch(...)
    {
        return -1;
    }    
    
    bool okay = false;

	if (cmdline.options & CommandLine::OPT_ANY_LIST) 
	    okay = List(cmdline.options);
	else
		okay = Process();

    ReleaseRedux();


    if (!okay)
    {
        cerr << "nothing extracted\n";
        cmdline.HelpOut();
        return -1;
    }

    return 0;
}

/////////////////////////////////////
bool ProcessDir(string filename,  const boost::regex& filter);
bool ProcessFile(boost::filesystem::path filename,  const boost::regex& filter);

bool Process()
{
    bool had_error = false;

    for (vector<string>::iterator f = cmdline.fbd_files.begin(); f!=cmdline.fbd_files.end();++f )
        had_error |= !ProcessDir(*f, cmdline.filter);

	return !had_error;
}

bool ProcessDir(string filename, const boost::regex& filter)
{
    namespace fs = boost::filesystem;
  
    fs::path basepath(filename);
    basepath.remove_filename();
	if (basepath.empty())
	{
		printf("no FDB File\n");
		return false;
	}

    bool something_extracted=false;
    
    if (fs::is_regular_file(filename))
    {
        printf("processing %s\n",filename.c_str());
        something_extracted |= ProcessFile(filename, filter);
    }
    else
    {
        for( fs::directory_iterator dir_iter(basepath) ; dir_iter != fs::directory_iterator(); ++dir_iter)
        {
            printf("processing %s\n",dir_iter->path().generic_string().c_str());
            something_extracted |= ProcessFile(dir_iter->path(), filter);
        }
    }

    return something_extracted;
}

bool ProcessFile(boost::filesystem::path filename, const boost::regex& filter)
{
    bool extracted = false;
    FDBPackage fdb( filename.generic_string().c_str() );

    FDBPackage::e_export_format ex_format=FDBPackage::EX_NONE;
    if (cmdline.options & CommandLine::OPT_RAW_DATA)      ex_format = (FDBPackage::e_export_format)(ex_format + FDBPackage::EX_RAW);
    if (cmdline.options & CommandLine::OPT_DB_SQLITE_OUT) ex_format = (FDBPackage::e_export_format)(ex_format + FDBPackage::EX_SQLITE);
    if (cmdline.options & CommandLine::OPT_DB_MYSQL_OUT)  ex_format = (FDBPackage::e_export_format)(ex_format + FDBPackage::EX_MYSQL);
    if (cmdline.options & CommandLine::OPT_LUA_OUT)       ex_format = (FDBPackage::e_export_format)(ex_format + FDBPackage::EX_LUA);
    if (cmdline.options & CommandLine::OPT_CSV_OUT)       ex_format = (FDBPackage::e_export_format)(ex_format + FDBPackage::EX_CSV);
    

    for (size_t id=0;id<fdb.GetFileCount();++id)
    {
        const char*fname = fdb.GetFileName(id);
        
        if (boost::regex_search(fname, filter, boost::match_any)) 
        { 
            boost::filesystem::path output(cmdline.output_directory);
            output /= fname; // TODO: get real name (in case of DB it may differ)

            boost::filesystem::create_directories(output.parent_path());


            if (! (cmdline.options & CommandLine::OPT_OVERWRITE))
            {
                if (boost::filesystem::exists(output))
                {
                    if (cmdline.options & CommandLine::OPT_NEVEROVERWRITE) continue;
                    char command=0;
                    while (string("ynas").find(command)== string::npos)
                    {
                        cout << fname << " exists.\n Overwrite? [y]es,[n]o,[a]lways yes,[s]kip all\n";
                        command=_getch();
                    }

                    cout <<command<<"\n";
                    if (command=='n') continue;
                    if (command=='s') { cmdline.options |= CommandLine::OPT_NEVEROVERWRITE; continue; }
                    if (command=='a') cmdline.options |= CommandLine::OPT_OVERWRITE;
                }
            }

            bool res = fdb.ExtractFile(id, output.generic_string().c_str(), ex_format );
            extracted |= res;

            if (cmdline.options & CommandLine::OPT_VERBOSE)
            {
                if (!res) cout << fname << "-ERROR\n";
                else      cout << fname <<"\n";
            }

        }
    }

    return extracted;
}

/////////////////////////////////////
bool ListDir(string filename,  const boost::regex& filter, uint16_t options);
bool ListFile(boost::filesystem::path filename,  const boost::regex& filter, uint16_t options);

bool List(uint16_t options)
{
    bool had_error = false;

    for (vector<string>::iterator f = cmdline.fbd_files.begin(); f!=cmdline.fbd_files.end();++f )
        had_error |= !ListDir(*f, cmdline.filter, options);

	return !had_error;
}

bool ListDir(string filename, const boost::regex& filter, uint16_t options)
{
    namespace fs = boost::filesystem;
  
    fs::path basepath(filename);
    basepath.remove_filename();
    
    bool something_extracted=false;
    
    if (fs::is_regular_file(filename))
    {
        printf("processing %s\n",filename.c_str());
        something_extracted |= ListFile(filename, filter, options);
    }
    else
    {
      for( fs::directory_iterator dir_iter(basepath) ; dir_iter != fs::directory_iterator() ; ++dir_iter)
      {
        printf("processing %s\n",dir_iter->path().c_str());
        something_extracted |= ListFile(dir_iter->path(), filter, options);
      }
    }

    return something_extracted;
}

bool ListFile(boost::filesystem::path filename, const boost::regex& filter, uint16_t options)
{
    bool extracted = false;
    FDBPackage fdb( filename.generic_string().c_str() );
	FDBPackage::file_info s_info;

    for (size_t id=0;id<fdb.GetFileCount();++id)
    {
        const char*fname = fdb.GetFileName(id);
        
        if (boost::regex_search(fname, filter, boost::match_any)) 
        { 
            boost::filesystem::path output(cmdline.output_directory);
            output /= fname; // TODO: get real name (in case of DB it may differ)


			cout << output.generic_string();

			if (options & CommandLine::OPT_LIST_ONLY_FULL)
			{
				fdb.GetFileInfo(id,s_info);
				cout <<"\t"<< s_info.size_uncomp;
				if (s_info.ftype==2) cout <<"\t"<<s_info.width<<"\t"<<s_info.height<<"\t"<< (unsigned)s_info.mipmapcount;
			}

			if (options & CommandLine::OPT_LIST_ONLY_WITH_CRC)
			{
				uint32_t crc32 = fdb.CalcCRC32(id);
				cout <<"\t"<< setw(8) << setfill('0') << hex << crc32;
			}

			if (options & CommandLine::OPT_LIST_ONLY_WITH_CRCRAW)
			{
				uint32_t crc32 = fdb.CalcCRC32Raw(id);
				cout <<"\t"<< setw(8) << setfill('0') << hex << crc32;
			}

			cout <<"\n";
            extracted = true;
        }
    }

    return extracted;
}

