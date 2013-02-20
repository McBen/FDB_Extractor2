//#include <windows.h>

#include "commandline.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "FDB_LIB.h"

#if WIN32
// FIXME
	#include <windows.h>
#endif


CommandLine cmdline;


using namespace std;

bool Process();
bool List(bool include_crc, bool full_info);

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

	if (cmdline.list_only || cmdline.list_only_with_crc|| cmdline.list_only_full) 
	    okay = List(cmdline.list_only_with_crc, cmdline.list_only_full);
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
    boost::filesystem::path basepath(filename);
    basepath.remove_filename();


    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

    hFind = FindFirstFile(filename.c_str(), &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE)  return false;

    bool something_extracted=false;
    do 
    {
        printf("processing %s\n",FindFileData.cFileName);
        something_extracted |= ProcessFile(basepath / FindFileData.cFileName, filter);
    } while (FindNextFile(hFind,&FindFileData));

    FindClose(hFind);

    return something_extracted;
}

bool ProcessFile(boost::filesystem::path filename, const boost::regex& filter)
{
    bool extracted = false;
    FDBPackage fdb( filename.generic_string().c_str() );

    FDBPackage::e_export_format ex_format=FDBPackage::EX_NONE;
    if (cmdline.raw_data)   ex_format = (FDBPackage::e_export_format)(ex_format + FDBPackage::EX_RAW);
    if (cmdline.db_sql_out) ex_format = (FDBPackage::e_export_format)(ex_format + FDBPackage::EX_SQLITE3);
    if (cmdline.lua_out)    ex_format = (FDBPackage::e_export_format)(ex_format + FDBPackage::EX_LUA);
    if (cmdline.csv_out)    ex_format = (FDBPackage::e_export_format)(ex_format + FDBPackage::EX_CSV);
    

    for (size_t id=0;id<fdb.GetFileCount();++id)
    {
        const char*fname = fdb.GetFileName(id);
        
        if (boost::regex_search(fname, filter, boost::match_any)) 
        { 
            boost::filesystem::path output(cmdline.output_directory);
            output /= fname; // TODO: get real name (in case of DB it may differ)

            boost::filesystem::create_directories(output.parent_path());


            if (!cmdline.overwrite)
            {
                if (boost::filesystem::exists(output))
                {
                    if (cmdline.neveroverwrite) continue;
                    char command=0;
                    while (string("ynas").find(command)== string::npos)
                    {
                        cout << fname << " exists.\n Overwrite? [y]es,[n]o,[a]lways yes,[s]kip all\n";
                        command=_getch();
                    }

                    cout <<command<<"\n";
                    if (command=='n') continue;
                    if (command=='s') { cmdline.neveroverwrite=true; continue; }
                    if (command=='a') cmdline.overwrite=true;
                }
            }

            bool res = fdb.ExtractFile(id, output.generic_string().c_str(), ex_format );
            extracted |= res;

            if (cmdline.verbose)
            {
                if (!res) cout << fname << "-ERROR\n";
                else      cout << fname <<"\n";
            }

        }
    }

    return extracted;
}

/////////////////////////////////////
bool ListDir(string filename,  const boost::regex& filter, bool include_crc, bool full_info);
bool ListFile(boost::filesystem::path filename,  const boost::regex& filter, bool include_crc, bool full_info);

bool List(bool include_crc, bool full_info)
{
    bool had_error = false;

    for (vector<string>::iterator f = cmdline.fbd_files.begin(); f!=cmdline.fbd_files.end();++f )
        had_error |= !ListDir(*f, cmdline.filter, include_crc,full_info);

	return !had_error;
}

bool ListDir(string filename, const boost::regex& filter, bool include_crc, bool full_info)
{
    boost::filesystem::path basepath(filename);
    basepath.remove_filename();


    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

    hFind = FindFirstFile(filename.c_str(), &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE)  return false;

    bool something_extracted=false;
    do 
    {
        something_extracted |= ListFile(basepath / FindFileData.cFileName, filter, include_crc,full_info);
    } while (FindNextFile(hFind,&FindFileData));

    FindClose(hFind);

    return something_extracted;
}

bool ListFile(boost::filesystem::path filename, const boost::regex& filter, bool include_crc, bool full_info)
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

			if (full_info)
			{
				fdb.GetFileInfo(id,s_info);
				cout <<"\t"<< s_info.size_uncomp;
				if (s_info.ftype==2) cout <<"\t"<<s_info.width<<"\t"<<s_info.height<<"\t"<< (unsigned)s_info.mipmapcount;
			}

			if (include_crc)
			{
				uint32_t crc32 = fdb.CalcCRC32(id);
				cout <<"\t"<< setw(8) << setfill('0') << hex << crc32;
			}
				
			cout <<"\n";
            extracted = true;
        }
    }

    return extracted;
}

